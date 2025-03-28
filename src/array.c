#include "array.h"
#include "value.h"
#include <stdlib.h>
#include <string.h>

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

struct value *
array_head (struct array *array)
{
  return value_copy (array->storage[0]);
}

struct array *
array_tail (struct array *array)
{
  return array_create (&array->storage[1], array->size - 1);
}

struct array *
array_push_back (struct array *array, struct value *value)
{
  struct value *storage[array->size + 1];

  memcpy (storage, array->storage, array->size * sizeof (struct value *));

  storage[array->size] = value;

  return array_create (storage, array->size + 1);
}

struct array *
array_push_front (struct array *array, struct value *value)
{
  struct value *storage[array->size + 1];

  memcpy (&storage[1], array->storage, array->size * sizeof (struct value *));

  storage[0] = value;

  return array_create (storage, array->size + 1);
}

int
array_eq (struct array *a, struct array *b)
{
  if (a->size != b->size)
    return 0;

  size_t size;

  if (a->size < b->size)
    size = a->size;
  else
    size = b->size;

  for (size_t i = 0; i < size; ++i)
    if (!value_eq (a->storage[i], b->storage[i]))
      return 0;

  return 1;
}

