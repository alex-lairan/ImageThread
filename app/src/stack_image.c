#include "stack_image.h"

#include <stdlib.h>

// function to create a stack of given capacity. It initializes size of
// stack as 0
StackImage* createStack(unsigned capacity) {
  StackImage* stack = (StackImage*)malloc(sizeof(StackImage));
  stack->capacity = capacity;
  stack->top = -1;
  stack->array = (ImageUnit**)malloc(stack->capacity * sizeof(ImageUnit*));
  return stack;
}

int isFull(StackImage* stack) {
  return stack->top == stack->capacity - 1;
}

int isEmpty(StackImage* stack) {
  return stack->top == -1;
}

bool push(StackImage* stack, ImageUnit* item) {
  if (isFull(stack))
    return false;
  stack->array[++stack->top] = item;

  return true;
}

ImageUnit* pop(StackImage* stack) {
  stack->top -= 1;
  return stack->array[stack->top + 1];
}

ImageUnit* peek(StackImage* stack) {
  return stack->array[stack->top];
}
