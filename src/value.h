#ifndef VALUE_H
#define VALUE_H

#include "error.h"

/* LATER
union value_entry
{
  struct error error;
  long i;
  double f;
  char *s;
  struct function *function;
  struct native *native;
  struct array *array;
  struct thunk *thunk;
};
*/

union value_entry
{
  struct error error;
  long i;
  double f;
  char *s;
};

enum value_type
{
  VALUE_NOTHING,
  VALUE_ERROR,
  VALUE_STRING,
  VALUE_INTEGER,
  VALUE_FLOAT,
  VALUE_ARRAY,
  VALUE_THUNK,
  VALUE_FUNCTION,
  VALUE_NATIVE,
};

struct value
{
  union value_entry value;
  enum value_type type;
  struct location location;
  size_t references;
};

const char *value_type_string (enum value_type);

/* `value_create` doesn't use arenas. Uses malloc / calloc, each allocated
 * value must have a corresponding `value_destroy` called to it. */
struct value *value_create (enum value_type, struct location);
struct value *value_create_e (struct error, struct location);
struct value *value_copy (struct value *);

/* Assumes everything can be destroyed inside value. */
void value_destroy (struct value *);

/* `value_retain` increases `value->references` by 1. */
void value_retain (struct value *);

/* `value_release` decreases `value->references` by 1. When reference count
 * reaches 0, it will implicitly call to `value_destroy`. ONLY release a value
 * that has been retained before. */
void value_release (struct value *);

#endif // VALUE_H


