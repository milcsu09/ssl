#ifndef NATIVE_H
#define NATIVE_H


#include "state.h"

#include <stdio.h>


struct frame;
struct vm;


typedef struct value *native_function (struct state *, struct value **);


struct native
{
  native_function *function;

  struct value *head;

  struct native *tail;

  usize remaining;
};


struct native *
native_create (struct state *state, native_function *function, usize remaining);

void
native_destroy (struct state *state, struct native *native);

void
native_mark (struct native *native);

void
native_show (FILE *stream, struct native *native);

usize
native_arity (struct native *native);


#endif // NATIVE_H

