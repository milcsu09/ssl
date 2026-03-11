#ifndef STATE_H
#define STATE_H


#include "error.h"


struct aa;
struct gc;
struct scope;
struct value;


#define state_unhealthy_return1(state)                                                             \
  do                                                                                               \
    if ((state)->unhealthy)                                                                        \
      return;                                                                                      \
  while (0)


#define state_unhealthy_return2(state, value)                                                      \
  do                                                                                               \
    if ((state)->unhealthy)                                                                        \
      return (value);                                                                              \
  while (0)


struct state
{
  struct aa *aa;
  struct gc *gc;

  bool unhealthy;

  usize allocs;
  usize frees;

  struct location location;
  struct error error;
};


struct state *
state_create (void);

void
state_destroy (struct state *state);


void
state_register_scope (struct state *state, struct scope *scope);

void
state_register_value (struct state *state, struct value *value);


void *
state_malloc (struct state *state, usize size);

void *
state_realloc (struct state *state, void *p, usize size);

void
state_free (struct state *state, void *p);


void *
state_aa_malloc (struct state *state, usize size);


void
state_set_location (struct state *state, struct location location);

void
state_set_error (struct state *state, const char *format, ...);


#endif // STATE_H

