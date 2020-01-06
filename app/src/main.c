#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <bitmap.h>
#include <stdint.h>

#define DIM 3
#define LENGTH DIM
#define OFFSET DIM /2

const float KERNEL[DIM][DIM] = {
	{ -1, -1,	-1 },
	{ -1,  8,	-1 },
	{ -1, -1,	-1 }
};

typedef struct Color_t {
	float Red;
	float Green;
	float Blue;
} Color_e;


void apply_effect(Image* original, Image* new_i);
void apply_effect(Image* original, Image* new_i) {

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

					c.Red += ((float) p->r) * KERNEL[a][b];
					c.Green += ((float) p->g) * KERNEL[a][b];
					c.Blue += ((float) p->b) * KERNEL[a][b];
				}
			}

			Pixel* dest = &new_i->pixel_data[y][x];
			dest->r = (uint8_t)  (c.Red <= 0 ? 0 : c.Red >= 255 ? 255 : c.Red);
			dest->g = (uint8_t) (c.Green <= 0 ? 0 : c.Green >= 255 ? 255 : c.Green);
			dest->b = (uint8_t) (c.Blue <= 0 ? 0 : c.Blue >= 255 ? 255 : c.Blue);
		}
	}
}

int main(int argc, char** argv) {
  if(argc != 3) {
		fprintf(stderr, "Error: You must provide two arguments (Given %d)\n", argc - 1);
		return -1;
  }

	char* bitmap_in = argv[1];
  char* bitmap_out = argv[2];

  Image image = open_bitmap(bitmap_in);
	Image new_i;
	apply_effect(&image, &new_i);
	save_bitmap(new_i, bitmap_out);

  return 0;
}
