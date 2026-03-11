#include "native.h"
#include "value.h"


struct native *
native_create (struct state *state, native_function *function, usize remaining)
{
  struct native *native;

  native = state_malloc (state, sizeof (struct native));

  state_unhealthy_return2 (state, NULL);

  native->function = function;

  native->remaining = remaining;

  return native;
}


void
native_destroy (struct state *state, struct native *native)
{
  state_free (state, native);
}


void
native_mark (struct native *native)
{
  for (struct native *p = native; p; p = p->tail)
    if (p->head)
      value_mark (p->head);
}


void
native_show (FILE *stream, struct native *native)
{
  UNUSED (native);

  fprintf (stream, "<native,%zu>", native->remaining);

  // fprintf (stream, "<native %p, remaining=%zu>", (void *)native, native->remaining);
}


usize
native_arity (struct native *native)
{
  struct native *p = native;

  while (p->tail)
    p = p->tail;

  return p->remaining;
}

