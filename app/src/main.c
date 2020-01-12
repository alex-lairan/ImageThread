#include "effects.h"
#include "stack_image.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <math.h>
#include <pthread.h>
#include <time.h>


typedef struct ProgramParams {
	char* dir_in;
	char* dir_out;
	int producers;

	float kernel[DIM][DIM];
} ProgramParams;

typedef struct StackMutex {
	pthread_mutex_t* lock;
	StackImage* stack;
} StackMutex;

typedef struct Processing {
	ProgramParams* params;

	size_t bmp_count;
	size_t bmp_done;
	char** files;

	pthread_cond_t awake_signal;
	StackMutex* stack_mutex;
} Processing;

typedef struct ProcessUnit {
	int number;

	int start;
	int end;

	Processing* data;
} ProcessUnit;

Processing* load_processing_data(ProgramParams* params) {
	DIR* bitmap_directory;
	bitmap_directory = opendir(params->dir_in);

	if(!bitmap_directory) { return NULL; } // Folder does not exist

	struct dirent *bitmap_file_dir;
	size_t file_count = 0;

	while((bitmap_file_dir = readdir(bitmap_directory)) != NULL) {
			char *dot = strrchr(bitmap_file_dir->d_name, '.');
			if (dot && !strcmp(dot, ".bmp")) { file_count += 1; }
	}

	rewinddir(bitmap_directory);

	char** file_names = (char**) malloc(sizeof(char*) * file_count);
	size_t current_pos = 0;

	while((bitmap_file_dir = readdir(bitmap_directory)) != NULL) {
		char *dot = strrchr(bitmap_file_dir->d_name, '.');
		if (dot && !strcmp(dot, ".bmp")) {
			file_names[current_pos] = bitmap_file_dir->d_name;
			current_pos += 1;
		}
	}

	StackMutex* stack_mutex = (StackMutex*)malloc(sizeof(StackMutex));
	printf("Create a stack 50\n");
	stack_mutex->stack = createStack(50);
	printf("Create a lock\n");
	stack_mutex->lock = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(stack_mutex->lock, NULL);

	Processing* processing = (Processing*) malloc(sizeof(Processing));
	processing->params = params;
	processing->bmp_count = file_count;
	processing->bmp_done = 0;
	processing->files = file_names;
	processing->stack_mutex = stack_mutex;
	pthread_cond_init(&processing->awake_signal, NULL);
	return processing;
}

void free_processing(Processing* processing) {
	freeStack(processing->stack_mutex->stack);
	pthread_mutex_destroy(processing->stack_mutex->lock);
	free(processing->stack_mutex->lock);

	for(size_t i = 0; i < processing->bmp_count; ++i) {
		free(processing->files[i]);
	}

	free(processing->files);

	pthread_cond_destroy(&processing->awake_signal);

	free(processing);
}


ImageUnit* process_image(Processing* processing, int i) {
	int char_len_in = strlen(processing->params->dir_in) + strlen(processing->files[i]) + 2;
	int char_len_out = strlen(processing->params->dir_out) + strlen(processing->files[i]) + 2;

	char* image_path_in = (char *)malloc(char_len_in);
	char* image_path_out = (char *)malloc(char_len_out);

	sprintf(image_path_in, "%s/%s", processing->params->dir_in, processing->files[i]);
	sprintf(image_path_out, "%s/%s", processing->params->dir_out, processing->files[i]);

	Image image = open_bitmap(image_path_in);
	Image* new_i = (Image*)malloc(sizeof(Image));

	apply_effect(&image, new_i, processing->params->kernel);

	free(image_path_in);

	ImageUnit* unit = (ImageUnit*)malloc(sizeof(ImageUnit));
	unit->image = new_i;
	unit->path = image_path_out;

	return unit;
}

void* do_processing_threaded(void* process_unit_raw) {
	ProcessUnit* process_unit = (ProcessUnit*) process_unit_raw;

	printf("\nWorker %d Start with %d\tEnd with %d\n", process_unit->number, process_unit->start, process_unit->end);

	for(size_t i = process_unit->start; i < process_unit->end; ++i) {
		ImageUnit* unit = process_image(process_unit->data, i);
		printf("\x1B[33mWorker %d\x1B[0m", process_unit->number);
		printf("\t\x1B[36mProcess image %d %s\x1B[0m\n", i, process_unit->data->files[i]);

		process_unit->data->stack_mutex->stack;

		pthread_mutex_lock(process_unit->data->stack_mutex->lock);
			push(process_unit->data->stack_mutex->stack, unit);
			pthread_cond_broadcast(&process_unit->data->awake_signal);
		pthread_mutex_unlock(process_unit->data->stack_mutex->lock);
	}
}

void* consumer(void* processing_raw) {
	Processing* processing = (Processing*) processing_raw;

	while(processing->bmp_done < processing->bmp_count) {
		pthread_mutex_lock(processing->stack_mutex->lock);
		pthread_cond_wait(&processing->awake_signal, processing->stack_mutex->lock);
		pthread_mutex_unlock(processing->stack_mutex->lock);

		while(!isEmpty(processing->stack_mutex->stack)) {
			pthread_mutex_lock(processing->stack_mutex->lock);
				ImageUnit* unit = pop(processing->stack_mutex->stack);
			pthread_mutex_unlock(processing->stack_mutex->lock);

			printf("\x1B[37mSave image %s\x1B[0m\n", unit->path);
			save_bitmap(*unit->image, unit->path);
			processing->bmp_done += 1;
		}
	}
}

int main(int argc, char** argv) {
	clock_t t = clock();
  if(argc != 5) {
		fprintf(stderr, "Error: You must provide four arguments (Given %d)\n", argc - 1);
		fprintf(stderr, "\timage_thread IN_FOLDER OUT_FOLDER CONSUMER_COUNT FILTER\n");
		fprintf(stderr, "Filters :\n\t- boxblur\n\t- sharpen\n\t- edgedetect\n");
		fprintf(stderr, "In case of non existing filter, just copy BMP files\n");
		return -1;
  }

	ProgramParams params = { .dir_in = argv[1], .dir_out = argv[2], .producers = atoi(argv[3]) };
	select_kernel(argv[4], params.kernel);
	Processing* processing = load_processing_data(&params);

	if(processing == NULL) {
		fprintf(stderr, "Error: The `in` folder doesn't exists\n");
		return -2;
	}

	printf("Process %d images with %d workers\n", processing->bmp_count, processing->params->producers);

	pthread_t* threads = (pthread_t*) malloc(sizeof(pthread_t) * processing->params->producers);
	ProcessUnit* units = (ProcessUnit*) malloc(sizeof(ProcessUnit) * processing->params->producers);

	clock_t mid_time = clock() - t;
	printf("\x1B[35mTaken \x1B[1;31m%f\x1B[0;35m seconds to execute before threads\n\x1B[0m", ((double)mid_time)/CLOCKS_PER_SEC);

	for(size_t i = 0; i < processing->params->producers; ++i) {
		float ratio = ceil(processing->bmp_count * 1.0 / processing->params->producers);
		size_t start = ratio * i;
		size_t end = fmin(ratio * (i + 1), processing->bmp_count);

		ProcessUnit unit = { .start = start, .end = end, .data = processing, .number=i };
		units[i] = unit;

		pthread_create(threads + i, NULL, do_processing_threaded, units + i);
	}

	pthread_t consumer_thread;
	pthread_create(&consumer_thread, NULL, consumer, processing);

	mid_time = clock() - t;
	printf("\x1B[35mTaken \x1B[1;31m%f\x1B[0;35m seconds to execute before join\n\x1B[0m", ((double)mid_time)/CLOCKS_PER_SEC);

	for(size_t i = 0; i < processing->params->producers; ++i) {
		pthread_join(threads[i], NULL);
	}
	pthread_join(consumer_thread, NULL);

	free(units);
	free(threads);

	clock_t last_time = clock() - t;
	double time_taken = ((double)last_time)/CLOCKS_PER_SEC; // in seconds

	printf("\x1B[35mTaken \x1B[1;31m%f\x1B[0;35m seconds to execute \n\x1B[0m", time_taken);

  return 0;
}
