#include "gc.h"
#include "memory.h"
#include "state.h"

#include <stdarg.h>
#include <stdio.h>


struct state *
state_create (void)
{
  struct state *state;

  state = malloc0 (sizeof (struct state));

  if (!state)
    return NULL;

  state->aa = aa_create (state);

  if (!state->aa)
    {
      free (state);

      return NULL;
    }

  // state->gc = gc_create (state, 128, 128);
  state->gc = gc_create (state, GC_THRESHOLD_MINIMUM, GC_THRESHOLD_MINIMUM);

  return state;
}


void
state_destroy (struct state *state)
{
  gc_destroy (state, state->gc);
  aa_destroy (state, state->aa);

  free (state);
}


void
state_register_scope (struct state *state, struct scope *scope)
{
  gc_register_scope (state->gc, scope);
}


void
state_register_value (struct state *state, struct value *value)
{
  gc_register_value (state->gc, value);
}


void *
state_malloc (struct state *state, usize size)
{
  void *p = malloc0 (size);

  if (!p)
    {
      state_set_error (state, "out-of-memory");

      return NULL;
    }

  state->allocs++;

  return p;
}


void *
state_realloc (struct state *state, void *p, usize size)
{
  p = realloc (p, size);

  if (!p)
    {
      state_set_error (state, "out-of-memory");

      return NULL;
    }

  state->allocs++;
  state->frees++;

  return p;
}


void
state_free (struct state *state, void *p)
{
  free (p);

  state->frees++;
}


void *
state_aa_malloc (struct state *state, usize size)
{
  void *p = aa_malloc (state, state->aa, size);

  if (!p)
    state_set_error (state, "out-of-memory");

  return p;
}


void
state_set_location (struct state *state, struct location location)
{
  state->location = location;
}


void
state_set_error (struct state *state, const char *format, ...)
{
  if (state->unhealthy)
    return;

  state->unhealthy = true;

  struct error error;

  va_list va;

  va_start (va, format);

  vsnprintf (error.message, sizeof error.message, format, va);

  va_end (va);

  state->error = error;
}

