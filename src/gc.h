#ifndef GC_H
#define GC_H


#include "state.h"


#define GC_THRESHOLD_MINIMUM 1024
#define GC_THRESHOLD_GROWTH  2


struct scope;
struct value;


struct gc
{
  struct scope *scope_root;
  usize scope_allocated;
  usize scope_threshold;

  struct value *value_root;
  usize value_allocated;
  usize value_threshold;

  usize runs;
};


struct gc *
gc_create (struct state *state, usize scope_threshold, usize value_threshold);

void
gc_destroy (struct state *state, struct gc *gc);

void
gc_register_scope (struct gc *gc, struct scope *scope);

void
gc_register_value (struct gc *gc, struct value *value);

bool
gc_need_scope (struct gc *gc);

bool
gc_need_value (struct gc *gc);

bool
gc_need (struct gc *gc);

void
gc_mark (struct gc *gc);

void
gc_collect (struct state *state, struct gc *gc);


#endif // GC_H

