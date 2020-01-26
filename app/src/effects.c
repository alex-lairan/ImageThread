#include "effects.h"

#include <string.h>
#include <omp.h>

#define LENGTH DIM
#define OFFSET DIM /2

void copy_kernel(const float from[DIM][DIM], float to[DIM][DIM]);

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


void inline apply_convolution(Color_e* c, int a, int b, int x, int y, Image* img, float kernel[DIM][DIM]) __attribute__((always_inline));
void apply_convolution(Color_e* restrict c, int a, int b, int x, int y, Image* restrict img, float kernel[DIM][DIM]) {
	int xn = x + a - OFFSET;
	int yn = y + b - OFFSET;

	Pixel* p = &img->pixel_data[yn][xn];

	c->Red += ((float) p->r) * kernel[a][b];
	c->Green += ((float) p->g) * kernel[a][b];
	c->Blue += ((float) p->b) * kernel[a][b];
}

void apply_effect(Image* original, Image* new_i, float kernel[DIM][DIM]) {

	int w = original->bmp_header.width;
	int h = original->bmp_header.height;

	*new_i = new_image(w, h, original->bmp_header.bit_per_pixel, original->bmp_header.color_planes);

	#pragma omp parallel for collapse(2)
	for (int y = OFFSET; y < h - OFFSET; y++) {
		// #pragma omp parallel for
		for (int x = OFFSET; x < w - OFFSET; x++) {
			Color_e c = { .Red = 0, .Green = 0, .Blue = 0};

			#pragma omp parallel for reduction(+:c) collapse(2)
			for(int alpha = 0; alpha < 3; ++alpha) {
				for(int beta = 0; beta < 3; ++beta) {
					apply_convolution(&c, alpha, beta, x, y, original, kernel);
				}
			}

			// apply_convolution(&c, 0, 0, x, y, original, kernel);
			// apply_convolution(&c, 0, 1, x, y, original, kernel);
			// apply_convolution(&c, 0, 2, x, y, original, kernel);

			// apply_convolution(&c, 1, 0, x, y, original, kernel);
			// apply_convolution(&c, 1, 1, x, y, original, kernel);
			// apply_convolution(&c, 1, 2, x, y, original, kernel);

			// apply_convolution(&c, 2, 0, x, y, original, kernel);
			// apply_convolution(&c, 2, 1, x, y, original, kernel);
			// apply_convolution(&c, 2, 2, x, y, original, kernel);

			Pixel* dest = &new_i->pixel_data[y][x];
			dest->r = (uint8_t)  (c.Red <= 0 ? 0 : c.Red >= 255 ? 255 : c.Red);
			dest->g = (uint8_t) (c.Green <= 0 ? 0 : c.Green >= 255 ? 255 : c.Green);
			dest->b = (uint8_t) (c.Blue <= 0 ? 0 : c.Blue >= 255 ? 255 : c.Blue);
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

void copy_kernel(const float from[DIM][DIM], float to[DIM][DIM]) {
	for(int i = 0; i < DIM; ++i) {
		for(int j = 0; j < DIM; ++j) {
			to[i][j] = from[i][j];
		}
	}
}
