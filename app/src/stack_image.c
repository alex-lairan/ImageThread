#include "stack_image.h"

#include <stdlib.h>

// function to create a stack of given capacity. It initializes size of
// stack as 0
StackImage* createStack(unsigned capacity) {
  StackImage* stack = (StackImage*)malloc(sizeof(StackImage));
  stack->capacity = capacity;
  stack->top = -1;
  stack->array = (Image**)malloc(stack->capacity * sizeof(Image*));
  return stack;
}

// Stack is full when top is equal to the last index
int isFull(StackImage* stack) {
  return stack->top == stack->capacity - 1;
}

// Stack is empty when top is equal to -1
int isEmpty(StackImage* stack) {
  return stack->top == -1;
}

// Function to add an item to stack.  It increases top by 1
bool push(StackImage* stack, Image* item) {
  if (isFull(stack))
    return false;
  stack->array[++stack->top] = item;

  return true;
}

// Function to remove an item from stack.  It decreases top by 1
Image* pop(StackImage* stack) {
  stack->top -= 1;
  return stack->array[stack->top + 1];
}

// Function to return the top from stack without removing it
Image* peek(StackImage* stack) {
  return stack->array[stack->top];
}
