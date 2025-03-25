#include "array.h"
#include "native.h"
#include <stdlib.h>

struct native *
native_create (native_c_function_t function)
{
  struct native *native;

  native = calloc (1, sizeof (struct native));

  native->arguments = array_create (NULL, 0);
  native->function = function;

  return native;
}

struct native *
native_copy (struct native *native)
{
  struct native *copy;

  copy = native_create (native->function);

  array_destroy (copy->arguments);

  copy->arguments = array_copy (native->arguments);

  return copy;
}

void
native_destroy (struct native *native)
{
  array_destroy (native->arguments);
  free (native);
}

int
native_match_argument (struct native *native, int i, enum value_type type)
{
  return value_match (native->arguments->storage[i], type);
}

