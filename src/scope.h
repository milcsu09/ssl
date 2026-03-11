#ifndef SCOPE_H
#define SCOPE_H


#include "common.h"


struct state;


struct scope_data
{
  char *key;

  struct value *value;
};


struct scope
{
  struct scope *next;
  struct scope *parent;

  usize size;
  usize capacity;

  bool marked;

  struct scope_data data[];
};


enum scope_set_result
{
  SCOPE_SET_OK,
  SCOPE_SET_REDEFINED,
};


enum scope_get_result
{
  SCOPE_GET_OK,
  SCOPE_GET_UNDEFINED,
};


struct scope *
scope_create (struct state *state, struct scope *parent, usize capacity);

void
scope_destroy (struct state *state, struct scope *scope);

void
scope_mark (struct scope *scope);

enum scope_set_result
scope_set (struct scope *scope, char *key, struct value *value);

enum scope_get_result
scope_get (struct scope *scope, char *key, struct value **value);


#endif // SCOPE_H

