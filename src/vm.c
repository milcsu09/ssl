#include "gc.h"
#include "lambda.h"
#include "list.h"
#include "program.h"
#include "scope.h"
#include "native.h"
#include "value.h"
#include "vm.h"


// If a native takes more arguemnts than NATIVE_ARGUMENT_STACK_SIZE, it's argument array is
// allocated on the heap.
#define NATIVE_ARGUMENT_STACK_SIZE 8


struct vm *
vm_create (struct state *state, usize frames_len_max)
{
  struct vm *vm;

  vm = state_malloc (state, sizeof (struct vm));

  state_unhealthy_return2 (state, NULL);

  vm->frames_len_max = frames_len_max;

  return vm;
}


void
vm_destroy (struct state *state, struct vm *vm)
{
  for (usize i = 0; i < vm->frames.len; ++i)
    frame_destroy (state, vm->frames.data[i]);

  da_clear (state, &vm->frames);

  state_free (state, vm);
}


void
vm_mark (struct vm *vm)
{
  for (usize i = vm->frames.len; i-- > 0;)
    frame_mark (vm->frames.data[i]);
}


void
vm_push (struct state *state, struct vm *vm, struct frame *frame)
{
  if (vm->frames.len >= vm->frames_len_max)
    {
      frame_destroy (state, frame);

      state_set_error (state, "Uh oh... stack frame overflow (%zu)", vm->frames_len_max);
      return;
    }

  if (vm->frames.len >= vm->frames_len_record)
    vm->frames_len_record = vm->frames.len;

  da_append (state, &vm->frames, frame);
}


struct frame *
vm_pop (struct state *state, struct vm *vm)
{
  if (vm->frames.len == 0)
    {
      state_set_error (state, "Uh oh... stack frame underflow");
      return NULL;
    }

  return vm->frames.data[--vm->frames.len];
}


struct frame *
vm_top (struct vm *vm)
{
  if (vm->frames.len)
    return vm->frames.data[vm->frames.len - 1];

  return NULL;
}


void
vm_set_top (struct vm *vm, struct frame *frame)
{
  if (vm->frames.len)
    vm->frames.data[vm->frames.len - 1] = frame;
}


void
vm_discard (struct state *state, struct vm *vm)
{
  struct frame *frame = vm_pop (state, vm);

  state_unhealthy_return1 (state);

  frame_destroy (state, frame);
}


void
vm_dump_stack (struct vm *vm)
{
  struct frame *frame = vm_top (vm);

  printf ("[");

  for (usize i = 0; i < frame->stack_head; ++i)
    {
      if (i > 0)
        printf (", ");

      value_show (stdout, frame->stack[i]);
    }

  printf ("]");
}


// NOTE: Does NOT check whether 'f' is a NATIVE or not.
struct value *
vm_call_native (struct state *state, struct value *f, struct value *x)
{
  struct native *previous = f->data.n;

  // If the native is not exhausted yet, return a partially applied native.
  if (previous->remaining - 1 != 0)
    {
      struct native *current;

      current = native_create (state, previous->function, previous->remaining - 1);

      state_unhealthy_return2 (state, NULL);

      current->head = x;
      current->tail = previous;

      struct value *result = value_box_n (state, current);

      state_unhealthy_return2 (state, NULL);

      return result;
    }

  usize arity = native_arity (previous);

  struct value *as_stack[NATIVE_ARGUMENT_STACK_SIZE];
  struct value **as;

  bool as_heap = arity > NATIVE_ARGUMENT_STACK_SIZE;

  if (as_heap)
    {
      as = state_malloc (state, sizeof (struct value *) * arity);

      state_unhealthy_return2 (state, NULL);
    }
  else
    as = as_stack;

  as[--arity] = x;

  for (struct native *p = previous; p->tail; p = p->tail)
    as[--arity] = p->head;

  struct value *result = previous->function (state, as);

  if (as_heap)
    state_free (state, as);

  state_unhealthy_return2 (state, NULL);

  return result;
}


void
vm_step (struct state *state, struct vm *vm)
{
  struct frame *frame = vm_top (vm);

  struct program *program = frame->program;

  struct scope *scope = frame->scope;

  if (frame->ip >= program->codes.len)
    {
      vm_pop (state, vm);

      state_unhealthy_return1 (state);

      return;
    }

  usize ip = frame->ip++;

  struct code code = program->codes.data[ip];

  insn i = code.i;

  struct location location = code.location;

  if (location.context)
    state_set_location (state, location);

  insn_data d = insn_get_data (i);

  switch (insn_get_kind (i))
    {
    case I_DROP:
      frame_pop (frame);
      break;

    case I_DUPLICATE:
      frame_push (frame, frame_top (frame));
      break;

    case I_LOAD_CONST:
      frame_push (frame, program->constants.data[d]);
      break;

    case I_LOAD_NAME:
      {
        char *name = program->names.data[d];

        struct value *value;

        enum scope_get_result result = scope_get (scope, name, &value);

        switch (result)
          {
          case SCOPE_GET_OK:
            frame_push (frame, value);
            break;

          case SCOPE_GET_UNDEFINED:
            state_set_error (state, "undefined %s", name);
            return;
          }
      }
      break;

    case I_STORE_NAME:
      {
        char *name = program->names.data[d];

        switch (scope_set (scope, name, frame_top (frame)))
          {
          case SCOPE_SET_OK:
            break;

          case SCOPE_SET_REDEFINED:
            state_set_error (state, "redefined %s", name);
            return;
          }
      }
      break;

    case I_STORE_NAME_DROP:
      {
        char *name = program->names.data[d];

        switch (scope_set (scope, name, frame_pop (frame)))
          {
          case SCOPE_SET_OK:
            break;

          case SCOPE_SET_REDEFINED:
            state_set_error (state, "redefined %s", name);
            return;
          }
      }
      break;

    case I_MAKE_LIST:
      {
        struct list *list = NULL;

        for (insn_data i = 0; i < d; ++i)
          {
            struct list *tail = list_create (state, frame_pop (frame), list);

            state_unhealthy_return1 (state);

            list = tail;
          }

        struct value *v = value_box_li (state, list);

        state_unhealthy_return1 (state);

        frame_push (frame, v);
      }
      break;

    case I_MAKE_LAMBDA:
      {
        struct lambda *la = lambda_create (state, frame_pop (frame)->data.p, frame->scope);

        state_unhealthy_return1 (state);

        struct value *v = value_box_la (state, la);

        state_unhealthy_return1 (state);

        frame_push (frame, v);
      }
      break;

    case I_UNCONS:
      {
        struct list *li = value_unbox_li (state, frame_pop (frame));

        state_unhealthy_return1 (state);

        list_retain (li->tail);

        struct value *tail = value_box_li (state, li->tail);

        state_unhealthy_return1 (state);

        frame_push (frame, tail);
        frame_push (frame, li->head);

        state_unhealthy_return1 (state);
      }
      break;

    case I_IS_TRUTHY:
      vm->truth_flag = value_bool (frame_pop (frame));
      break;

    case I_IS_EQUAL:
      {
        struct value *b = frame_pop (frame);
        struct value *a = frame_top (frame);

        vm->truth_flag = value_compare_eq (a, b);
      }
      break;

    case I_IS_CONS:
      {
        struct value *a = frame_top (frame);

        vm->truth_flag = a->kind == VALUE_LIST && a->data.li;
      }
      break;

    case I_JUMP:
      frame->ip += d;
      break;

    case I_JUMP_FALSE:
      if (vm->truth_flag == false)
        frame->ip += d;
      break;

    case I_CALL:
      {
        struct value *x = frame_pop (frame);
        struct value *f = frame_pop (frame);

        value_expect (state, f, 2, VALUE_LAMBDA, VALUE_NATIVE);

        state_unhealthy_return1 (state);

        switch (f->kind)
          {
          case VALUE_LAMBDA:
            {
              struct lambda *la = f->data.la;

              struct scope *local = scope_create (state, la->scope, la->program->scope_size);

              state_unhealthy_return1 (state);

              struct frame *child = frame_create (state, la->program, local);

              state_unhealthy_return1 (state);

              frame_push (child, x);

              vm_push (state, vm, child);

              state_unhealthy_return1 (state);
            }
            break;

          case VALUE_NATIVE:
            {
              struct value *result = vm_call_native (state, f, x);

              state_unhealthy_return1 (state);

              frame_push (frame, result);
            }
            break;

          default:
            break;
          }
      }
      break;

    case I_CALL_TAIL:
      {
        struct value *x = frame_pop (frame);
        struct value *f = frame_pop (frame);

        value_expect (state, f, 2, VALUE_LAMBDA, VALUE_NATIVE);

        state_unhealthy_return1 (state);

        switch (f->kind)
          {
          case VALUE_LAMBDA:
            {
              struct lambda *la = f->data.la;

              frame = frame_reserve (state, frame, la->program->stack_size);

              state_unhealthy_return1 (state);

              vm_set_top (vm, frame);

              struct scope *local;

              if (frame->scope->capacity >= la->program->scope_size)
                {
                  frame->scope->parent = la->scope;
                  frame->scope->size = 0;

                  local = frame->scope;
                }
              else
                {
                  local = scope_create (state, la->scope, la->program->scope_size);

                  state_unhealthy_return1 (state);
                }

              frame->program = la->program;
              frame->ip = 0;
              frame->scope = local;
              frame->stack_head = 0;

              frame_push (frame, x);

              // NOTE: Same safe point as I_RETURN; all values reachable.
              if (gc_need (state->gc))
                {
                  vm_mark (vm);
                  gc_collect (state, state->gc);
                }
            }
            break;

          case VALUE_NATIVE:
            {
              struct value *result = vm_call_native (state, f, x);

              state_unhealthy_return1 (state);

              frame_push (frame, result);
            }
            break;

          default:
            break;
          }
      }
      break;

    case I_CALL_BINARY:
      {
        struct value *as[2];

        as[1] = frame_pop (frame); // Right
        as[0] = frame_pop (frame); // Left

        struct native *f = value_unbox_n (state, frame_pop (frame));

        state_unhealthy_return1 (state);

        struct value *result = f->function (state, as);

        state_unhealthy_return1 (state);

        frame_push (frame, result);
      }
      break;

    case I_RETURN:
      {
        struct value *x = frame_pop (frame);

        if (frame->stack_head != 0)
          {
            fprintf (stderr, "\033[95mWarning\033[0m: returning with %ld value(s) still on stack.\n",
                     frame->stack_head);
          }

        vm_discard (state, vm);

        state_unhealthy_return1 (state);

        struct frame *parent = vm_top (vm);

        frame_push (parent, x);

        // NOTE: Return is a safe point; every needed value is reachable.
        if (gc_need (state->gc))
          {
            vm_mark (vm);

            gc_collect (state, state->gc);
          }
      }
      break;
    }
}


void
vm_execute (struct state *state, struct vm *vm)
{
  while (vm_top (vm))
    {
      vm_step (state, vm);

      state_unhealthy_return1 (state);
    }
}

