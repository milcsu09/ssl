#include "gc.h"
#include "scope.h"
#include "value.h"

#include "list.h"


struct gc *
gc_create (struct state *state, usize scope_threshold, usize value_threshold)
{
  struct gc *gc;

  gc = state_aa_malloc (state, sizeof (struct gc));

  state_unhealthy_return2 (state, NULL);

  gc->scope_threshold = scope_threshold;
  gc->value_threshold = value_threshold;

  return gc;
}


void
gc_destroy_scope (struct state *state, struct gc *gc)
{
  struct scope *p = gc->scope_root;

  while (p)
    {
      struct scope *q = p->next;

      scope_destroy (state, p);

      p = q;
    }
}


void
gc_destroy_value (struct state *state, struct gc *gc)
{
  struct value *p = gc->value_root;

  while (p)
    {
      struct value *q = p->next;

      value_destroy (state, p);

      p = q;
    }
}


void
gc_destroy (struct state *state, struct gc *gc)
{
  gc_destroy_scope (state, gc);
  gc_destroy_value (state, gc);
}


void
gc_register_scope (struct gc *gc, struct scope *scope)
{
  scope->next = gc->scope_root;

  gc->scope_root = scope;

  gc->scope_allocated++;
}


void
gc_register_value (struct gc *gc, struct value *value)
{
  value->next = gc->value_root;

  gc->value_root = value;

  gc->value_allocated++;
}


bool
gc_need_scope (struct gc *gc)
{
  return gc->scope_allocated >= gc->scope_threshold;
}


bool
gc_need_value (struct gc *gc)
{
  return gc->value_allocated >= gc->value_threshold;
}


bool
gc_need (struct gc *gc)
{
  return gc_need_scope (gc) || gc_need_value (gc);
}


void
gc_mark (struct gc *gc)
{
  for (struct scope *p = gc->scope_root; p; p = p->next)
    scope_mark (p);
}


void
gc_collect_scope (struct state *state, struct gc *gc)
{
  struct scope **p = &gc->scope_root;

  while (*p)
    {
      if (!(*p)->marked)
        {
          struct scope *q = *p;

          *p = q->next;

          scope_destroy (state, q);

          gc->scope_allocated--;
        }
      else
        {
          (*p)->marked = false;

          p = &(*p)->next;
        }
    }
}


void
gc_collect_value (struct state *state, struct gc *gc)
{
  struct value **p = &gc->value_root;

  while (*p)
    {
      if (!(*p)->marked)
        {
          struct value *q = *p;

          *p = q->next;

          value_destroy (state, q);

          gc->value_allocated--;
        }
      else
        {
          value_unmark (*p);

          p = &(*p)->next;
        }
    }
}


void
gc_collect (struct state *state, struct gc *gc)
{
  if (gc_need (gc))
    {
      gc_collect_scope (state, gc);

      gc->scope_threshold = max (GC_THRESHOLD_MINIMUM, gc->scope_allocated * GC_THRESHOLD_GROWTH);

      gc_collect_value (state, gc);

      gc->value_threshold = max (GC_THRESHOLD_MINIMUM, gc->value_allocated * GC_THRESHOLD_GROWTH);

      gc->runs++;
    }
}

