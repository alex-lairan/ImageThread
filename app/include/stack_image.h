#ifndef __STACK_IMAGE_H__
#define __STACK_IMAGE_H__

#include <bitmap.h>
#include <stdbool.h>

typedef struct StackImage {
    int top;
    unsigned capacity;
    Image** array;
} StackImage;

StackImage* createStack(unsigned capacity);
int isFull(StackImage* stack);
int isEmpty(StackImage* stack);
bool push(StackImage* stack, Image* item);
Image* pop(StackImage* stack);
Image* peek(StackImage* stack);

#endif /* __STACK_IMAGE_H__ */
