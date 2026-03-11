#include "scope.h"
#include "state.h"
#include "value.h"

#include <assert.h>
#include <string.h>


struct scope *
scope_create (struct state *state, struct scope *parent, usize capacity)
{
  struct scope *scope;

  scope = state_malloc (state, sizeof (struct scope) + sizeof (struct scope_data) * capacity);

  state_unhealthy_return2 (state, NULL);

  state_register_scope (state, scope);

  scope->parent = parent;

  scope->capacity = capacity;

  return scope;
}


void
scope_destroy (struct state *state, struct scope *scope)
{
  state_free (state, scope);
}


void
scope_mark (struct scope *scope)
{
  while (scope)
    {
      scope->marked = true;

      for (usize i = 0; i < scope->size; ++i)
        value_mark (scope->data[i].value);

      scope = scope->parent;
    }
}


enum scope_set_result
scope_set (struct scope *scope, char *key, struct value *value)
{
  for (usize i = 0; i < scope->size; ++i)
    if (strcmp (scope->data[i].key, key) == 0)
      {
        scope->data[i].value = value;

        return SCOPE_SET_OK;
      }

  struct scope_data data;

  data.key = key;
  data.value = value;

  scope->data[scope->size++] = data;

  return SCOPE_SET_OK;
}


enum scope_get_result
scope_get (struct scope *scope, char *key, struct value **value)
{
  for (usize i = 0; i < scope->size; ++i)
    if (strcmp (scope->data[i].key, key) == 0)
      {
        *value = scope->data[i].value;
        return SCOPE_GET_OK;
      }

  if (scope->parent)
    return scope_get (scope->parent, key, value);

  return SCOPE_GET_UNDEFINED;
}

