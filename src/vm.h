#ifndef VM_H
#define VM_H


#include "frame.h"
#include "state.h"


// struct vm
// {
//   usize stack_head;
//   usize stack_size;
// 
//   struct frame *stack[];
// };


struct vm
{
  usize frames_len_max;

  usize frames_len_record;

  struct
  {
    struct frame **data;
    usize len;
    usize cap;
  } frames;

  bool truth_flag;
};


struct vm *vm_create (struct state *, usize);

void vm_destroy (struct state *, struct vm *);

void vm_mark (struct vm *);

void vm_push (struct state *, struct vm *, struct frame *);

struct frame *vm_pop (struct state *, struct vm *);

struct frame *vm_top (struct vm *);

void vm_discard (struct state *, struct vm *);

void vm_step (struct state *, struct vm *);

void vm_execute (struct state *, struct vm *);


#endif // VM_H

