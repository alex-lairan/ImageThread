#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <bitmap.h>
#include <stdint.h>
#include <string.h>

#define DIM 3
#define LENGTH DIM
#define OFFSET DIM /2

const float KERNEL_IDENTITY[DIM][DIM] = {
	{  0,  0,	 0 },
	{  0,  1,	 0 },
	{  0,  0,	 0 }
};

const float KERNEL_EDGE_DETECT[DIM][DIM] = {
	{ -1, -1,	-1 },
	{ -1,  8,	-1 },
	{ -1, -1,	-1 }
};

const float KERNEL_SHARPEN[DIM][DIM] = {
	{  0, -1,	 0 },
	{ -1,  5,	-1 },
	{  0, -1,	 0 }
};

const float KERNEL_BOX_BLUR[DIM][DIM] = {
	{ 1.0/9, 1.0/9, 1.0/9 },
	{ 1.0/9, 1.0/9, 1.0/9 },
	{ 1.0/9, 1.0/9, 1.0/9 }
};

typedef struct Color_t {
	float Red;
	float Green;
	float Blue;
} Color_e;


void apply_effect(Image* original, Image* new_i, float kernel[DIM][DIM]);
void apply_effect(Image* original, Image* new_i, float kernel[DIM][DIM]) {

	int w = original->bmp_header.width;
	int h = original->bmp_header.height;

	*new_i = new_image(w, h, original->bmp_header.bit_per_pixel, original->bmp_header.color_planes);

	for (int y = OFFSET; y < h - OFFSET; y++) {
		for (int x = OFFSET; x < w - OFFSET; x++) {
			Color_e c = { .Red = 0, .Green = 0, .Blue = 0};

			for(int a = 0; a < LENGTH; a++){
				for(int b = 0; b < LENGTH; b++){
					int xn = x + a - OFFSET;
					int yn = y + b - OFFSET;

					Pixel* p = &original->pixel_data[yn][xn];

					c.Red += ((float) p->r) * kernel[a][b];
					c.Green += ((float) p->g) * kernel[a][b];
					c.Blue += ((float) p->b) * kernel[a][b];
				}
			}

			Pixel* dest = &new_i->pixel_data[y][x];
			dest->r = (uint8_t)  (c.Red <= 0 ? 0 : c.Red >= 255 ? 255 : c.Red);
			dest->g = (uint8_t) (c.Green <= 0 ? 0 : c.Green >= 255 ? 255 : c.Green);
			dest->b = (uint8_t) (c.Blue <= 0 ? 0 : c.Blue >= 255 ? 255 : c.Blue);
		}
	}
}

void copy_kernel(const float from[DIM][DIM], float to[DIM][DIM]) {
	for(int i = 0; i < DIM; ++i) {
		for(int j = 0; j < DIM; ++j) {
			to[i][j] = from[i][j];
		}
	}
}

void select_kernel(char* effect, float kernel[DIM][DIM]) {
	if(strcmp(effect, "boxblur") == 0) {
		copy_kernel(KERNEL_BOX_BLUR, kernel);
	} else if(strcmp(effect, "sharpen") == 0) {
		copy_kernel(KERNEL_SHARPEN, kernel);
	} else if(strcmp(effect, "edgedetect") == 0) {
		copy_kernel(KERNEL_EDGE_DETECT, kernel);
	} else {
		copy_kernel(KERNEL_IDENTITY, kernel);
	}
}

int main(int argc, char** argv) {
  if(argc != 5) {
		fprintf(stderr, "Error: You must provide four arguments (Given %d)\n", argc - 1);
		return -1;
  }

	char* bitmap_in = argv[1];
  char* bitmap_out = argv[2];
	int producers = atoi(argv[3]);
	char* effect = argv[4];

	float kernel[DIM][DIM];
	select_kernel(effect, kernel);

  Image image = open_bitmap(bitmap_in);
	Image new_i;
	apply_effect(&image, &new_i, kernel);
	save_bitmap(new_i, bitmap_out);

  return 0;
}
