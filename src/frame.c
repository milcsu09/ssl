#include "frame.h"
#include "program.h"
#include "scope.h"
#include "value.h"


struct frame *
frame_create (struct state *state, struct program *program, struct scope *scope)
{
  usize stack_size = program->stack_size;

  struct frame *frame;

  usize stack_bytes = sizeof (struct value *) * stack_size;

  frame = state_malloc (state, sizeof (struct frame) + stack_bytes);

  state_unhealthy_return2 (state, NULL);

  frame->program = program;

  frame->scope = scope;

  return frame;
}


struct frame *
frame_reserve (struct state *state, struct frame *frame, isize stack_size)
{
  if (frame->program->stack_size < stack_size)
    {
      usize stack_bytes = sizeof (struct value *) * stack_size;

      return state_realloc (state, frame, sizeof (struct frame) + stack_bytes);
    }

  return frame;
}


void
frame_destroy (struct state *state, struct frame *frame)
{
  state_free (state, frame);
}


void
frame_mark (struct frame *frame)
{
  program_mark (frame->program);

  for (usize i = 0; i < frame->stack_head; ++i)
    value_mark (frame->stack[i]);

  scope_mark (frame->scope);
}


void
frame_push (struct frame *frame, struct value *value)
{
  // NOTE: Overflow should never happen. If it does => compiler bug.
  frame->stack[frame->stack_head++] = value;
}


struct value *
frame_pop (struct frame *frame)
{
  // NOTE: Underflow should never happen. If it does => compiler bug.
  return frame->stack[--frame->stack_head];
}


struct value *
frame_top (struct frame *frame)
{
  if (frame->stack_head)
    return frame->stack[frame->stack_head - 1];

  return NULL;
}

