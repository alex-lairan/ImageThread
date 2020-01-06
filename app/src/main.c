#include "effects.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

typedef struct Processing {
	char* dir_in;
	char* dir_out;

	size_t bmp_count;
	char** files;
} Processing;

Processing* load_processing_data(char* dir_in, char* dir_out) {
	DIR* bitmap_directory;
	bitmap_directory = opendir(dir_in);

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

	Processing* processing = (Processing*) malloc(sizeof(Processing));
	processing->dir_in = dir_in;
	processing->dir_out = dir_out;
	processing->bmp_count = file_count;
	processing->files = file_names;

	return processing;
}

int main(int argc, char** argv) {
  if(argc != 5) {
		fprintf(stderr, "Error: You must provide four arguments (Given %d)\n", argc - 1);
		fprintf(stderr, "\timage_thread IN_FOLDER OUT_FOLDER CONSUMER_COUNT FILTER\n");
		fprintf(stderr, "Filters :\n\t- boxblur\n\t- sharpen\n\t- edgedetect\n");
		fprintf(stderr, "In case of non existing filter, just copy BMP files\n");
		return -1;
  }

	char* bitmap_in = argv[1];
  char* bitmap_out = argv[2];
	int producers = atoi(argv[3]);
	char* effect = argv[4];

	float kernel[DIM][DIM];
	select_kernel(effect, kernel);

	Processing* processing = load_processing_data(bitmap_in, bitmap_out);

	if(processing == NULL) {
		fprintf(stderr, "Error: The `in` folder doesn't exists\n");
		return -2;
	}

	printf("Process %d images\n", processing->bmp_count);

	for(int i = 0; i < processing->bmp_count; ++i) {
		int char_len_in = strlen(processing->dir_in) + strlen(processing->files[i]) + 2;
		int char_len_out = strlen(processing->dir_out) + strlen(processing->files[i]) + 2;

		char* image_path_in = (char *)malloc(char_len_in);
		char* image_path_out = (char *)malloc(char_len_out);

		sprintf(image_path_in, "%s/%s", processing->dir_in, processing->files[i]);
		sprintf(image_path_out, "%s/%s", processing->dir_out, processing->files[i]);

		Image image = open_bitmap(image_path_in);
		Image new_i;
		apply_effect(&image, &new_i, kernel);
		save_bitmap(new_i, image_path_out);

		free(image_path_in);
		free(image_path_out);
	}

  return 0;
}
