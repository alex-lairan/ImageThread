#ifndef __EFFECTS_H__
#define __EFFECTS_H__

#include <bitmap.h>

#define DIM 3

typedef struct Color_t {
	float Red;
	float Green;
	float Blue;
} Color_e;

void apply_effect(Image* original, Image* new_i, float kernel[DIM][DIM]);
void select_kernel(char* effect, float kernel[DIM][DIM]);

#endif /* __EFFECTS_H__ */
