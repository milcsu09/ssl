#ifndef FRAME_H
#define FRAME_H


#include "state.h"


struct program;
struct scope;
struct value;


struct frame
{
  // NOTE: Program's lifetime is NOT handled by 'struct frame'.
  struct program *program;

  usize ip;

  struct scope *scope;

  usize stack_head;

  struct value *stack[];
};


struct frame *
frame_create (struct state *state, struct program *program, struct scope *scope);

struct frame *
frame_reserve (struct state *state, struct frame *frame, isize stack_size);

void
frame_destroy (struct state *state, struct frame *frame);

void
frame_mark (struct frame *frame);

void
frame_push (struct frame *frame, struct value *value);

struct value *
frame_pop (struct frame *frame);

struct value *
frame_top (struct frame *frame);


#endif // FRAME_H

