#ifndef ARRAY_H
#define ARRAY_H

#include <stdlib.h>

struct value;

struct array
{
  struct value **storage;
  size_t size;
};

struct array *array_create (struct value **, size_t);
struct array *array_copy (struct array *);

void array_destroy (struct array *);

struct array *array_push_back (struct array *, struct value *);
struct array *array_push_front (struct array *, struct value *);

#endif // ARRAY_H

