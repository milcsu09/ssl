#ifndef NATIVE_H
#define NATIVE_H

#include "value.h"

typedef struct value *(*native_c_function_t) (struct value *);

struct native
{
  struct array *arguments;
  native_c_function_t function;
};

struct native *native_create (native_c_function_t);
struct native *native_copy (struct native *);

void native_destroy (struct native *);

int native_match_argument (struct native *, int, enum value_type);

#endif // NATIVE_H

