#include "array.h"
#include "value.h"
#include <stdlib.h>

struct array *
array_create (struct value **storage, size_t size)
{
  struct array *array;

  array = calloc (1, sizeof (struct array));

  array->storage = calloc (size, sizeof (struct value *));
  array->size = size;

  for (size_t i = 0; i < size; ++i)
    array->storage[i] = value_copy (storage[i]);

  return array;
}

struct array *
array_copy (struct array *array)
{
  return array_create (array->storage, array->size);
}

void
array_destroy (struct array *array)
{
  for (size_t i = 0; i < array->size; ++i)
    value_destroy (array->storage[i]);
  free (array->storage);
  free (array);
}

