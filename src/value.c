#include "array.h"
#include "string.h"
#include "value.h"
#include <stdio.h>
#include <stdlib.h>

static const char *const VALUE_TYPE_STRING[] = {
  "nothing",
  "error",
  "string",
  "integer",
  "float",
  "array",
  "thunk",
  "function",
  "native",
};

const char *
value_type_string (enum value_type type)
{
  return VALUE_TYPE_STRING[type];
}

struct value *
value_create (enum value_type type, struct location location)
{
  struct value *value;

  value = calloc (1, sizeof (struct value));

  value->type = type;
  value->location = location;

  return value;
}

struct value *
value_create_e (struct error error, struct location location)
{
  struct value *value;

  value = value_create (VALUE_ERROR, location);

  value->value.error = error;

  return value;
}

struct value *
value_copy (struct value *value)
{
  struct value *copy;

  copy = value_create (value->type, value->location);

  switch (value->type)
    {
    case VALUE_NOTHING:
      break;
    case VALUE_ERROR:
      copy->value.error = value->value.error;
      break;
    case VALUE_STRING:
      copy->value.s = string_copy (value->value.s, NULL);
      break;
    case VALUE_INTEGER:
      copy->value.i = value->value.i;
      break;
    case VALUE_FLOAT:
      copy->value.f = value->value.f;
      break;
    case VALUE_ARRAY:
      copy->value.array = array_copy (value->value.array);
      break;
    case VALUE_THUNK:
      break;
    case VALUE_FUNCTION:
      break;
    case VALUE_NATIVE:
      break;
    }

  return copy;
}

void
value_destroy (struct value *value)
{
  switch (value->type)
    {
    case VALUE_NOTHING:
      break;
    case VALUE_ERROR:
      break;
    case VALUE_STRING:
      free (value->value.s);
      break;
    case VALUE_INTEGER:
      break;
    case VALUE_FLOAT:
      break;
    case VALUE_ARRAY:
      array_destroy (value->value.array);
      break;
    case VALUE_THUNK:
      /* thunk_destroy */
      break;
    case VALUE_FUNCTION:
      /* function_destroy */
      break;
    case VALUE_NATIVE:
      /* native_destroy */
      break;
    }

  free (value);
}

void
value_retain (struct value *value)
{
  value->references++;
}

void
value_release (struct value *value)
{
  value->references--;
  if (value->references == 0)
    value_destroy (value);
}

int
value_match (struct value *value, enum value_type type)
{
  return value->type == type;
}

int
value_match_error (struct value *value)
{
  return value_match (value, VALUE_ERROR);
}

void
value_debug_print (struct value *value)
{
  switch (value->type)
    {
    case VALUE_STRING:
      printf ("\"%s\"", value->value.s);
      break;
    case VALUE_INTEGER:
      printf ("%ld", value->value.i);
      break;
    case VALUE_FLOAT:
      printf ("%g", value->value.f);
      break;
    case VALUE_ARRAY:
      /* LATER. */
      break;
    default:
      printf ("%s", value_type_string (value->type));
      break;
    }
}

