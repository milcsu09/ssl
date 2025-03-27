#include "array.h"
#include "function.h"
#include "native.h"
#include "string.h"
#include "thunk.h"
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
      copy->value.thunk = thunk_copy (value->value.thunk);
      break;
    case VALUE_FUNCTION:
      copy->value.function = function_copy (value->value.function);
      break;
    case VALUE_NATIVE:
      copy->value.native = native_copy (value->value.native);
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
      thunk_destroy (value->value.thunk);
      break;
    case VALUE_FUNCTION:
      function_destroy (value->value.function);
      break;
    case VALUE_NATIVE:
      native_destroy (value->value.native);
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
  value_drop (value);
}

void
value_drop (struct value *value)
{
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

// struct value *
// value_invoke (struct value *f, struct value *argument)
// {
// }

int
value_bool (struct value *value)
{
  switch (value->type)
    {
    case VALUE_NOTHING:
    case VALUE_ERROR:
      return 0;
    case VALUE_STRING:
      return *value->value.s != '\0';
    case VALUE_INTEGER:
      return value->value.i != 0;
    case VALUE_FLOAT:
      return value->value.f != 0;
    case VALUE_ARRAY:
      return value->value.array->size != 0;
    case VALUE_THUNK:
    case VALUE_FUNCTION:
    case VALUE_NATIVE:
      return 1;
    }
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
      {
        printf ("[");
        for (size_t i = 0; i < value->value.array->size; ++i)
          {
            value_debug_print (value->value.array->storage[i]);
            if (i < value->value.array->size - 1)
              printf (",");
          }

        printf ("]");
      }
      break;
    default:
      printf ("%s", value_type_string (value->type));
      break;
    }
}

