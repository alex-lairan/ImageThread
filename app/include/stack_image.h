#ifndef __STACK_IMAGE_H__
#define __STACK_IMAGE_H__

#include <bitmap.h>
#include <stdbool.h>

typedef struct ImageUnit {
  Image* image;
  char* path;
} ImageUnit;

typedef struct StackImage {
  int top;
  unsigned capacity;
  ImageUnit** array;
} StackImage;

StackImage* createStack(unsigned capacity);
void freeStack(StackImage* stack);
int isFull(StackImage* stack);
int isEmpty(StackImage* stack);
bool push(StackImage* stack, ImageUnit* item);
ImageUnit* pop(StackImage* stack);
ImageUnit* peek(StackImage* stack);

#endif /* __STACK_IMAGE_H__ */
