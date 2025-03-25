#include "array.h"
#include "standard.h"
#include "native.h"
#include "value.h"
#include <stdio.h>

struct value *
standard_error_expect_base (struct value *curry, const char *a, const char *b)
{
  return value_create_e (error_create ("expected `%s`, got `%s`", a, b),
                         curry->location);
}

struct value *
standard_error_expect_type (struct value *curry, enum value_type t1,
                            enum value_type t2)
{
  const char *a = value_type_string (t1);
  const char *b = value_type_string (t2);
  return standard_error_expect_base (curry, a, b);
}

struct value *
standard_f_f (struct value *curry)
{
  struct native *native = curry->value.native;
  if (native->arguments->size < 3)
    return curry;

  value_debug_print (native->arguments->storage[0]);
  printf (", ");
  value_debug_print (native->arguments->storage[1]);
  printf (", ");
  value_debug_print (native->arguments->storage[2]);
  printf ("\n");

  return value_create (VALUE_NOTHING, curry->location);
}

struct value *
standard_f_print (struct value *curry)
{
  struct native *native = curry->value.native;
  /* single-argument */

  if (native_match_argument (native, 0, VALUE_STRING))
    printf ("%s\n", native->arguments->storage[0]->value.s);
  else
    {
      value_debug_print (native->arguments->storage[0]);
      printf ("\n");
    }

  return value_create (VALUE_NOTHING, curry->location);
}

struct value *
standard_f_add (struct value *curry)
{
  struct native *native = curry->value.native;
  if (native->arguments->size < 2)
    return curry;

  struct value *result;

  result = value_create (VALUE_INTEGER, curry->location);

  result->value.i = native->arguments->storage[0]->value.i;
  result->value.i += native->arguments->storage[1]->value.i;

  return result;
}

struct value *
standard_f_sub (struct value *curry)
{
  struct native *native = curry->value.native;
  if (native->arguments->size < 2)
    return curry;

  struct value *result;

  result = value_create (VALUE_INTEGER, curry->location);

  result->value.i = native->arguments->storage[0]->value.i;
  result->value.i -= native->arguments->storage[1]->value.i;

  return result;
}

struct value *
standard_f_mul (struct value *curry)
{
  struct native *native = curry->value.native;
  if (native->arguments->size < 2)
    return curry;

  struct value *result;

  result = value_create (VALUE_INTEGER, curry->location);

  result->value.i = native->arguments->storage[0]->value.i;
  result->value.i *= native->arguments->storage[1]->value.i;

  return result;
}

struct value *
standard_f_div (struct value *curry)
{
  struct native *native = curry->value.native;
  if (native->arguments->size < 2)
    return curry;

  struct value *result;

  result = value_create (VALUE_INTEGER, curry->location);

  result->value.i = native->arguments->storage[0]->value.i;
  result->value.i /= native->arguments->storage[1]->value.i;

  return result;
}

struct value *
standard_f_mod (struct value *curry)
{
  struct native *native = curry->value.native;
  if (native->arguments->size < 2)
    return curry;

  struct value *result;

  result = value_create (VALUE_INTEGER, curry->location);

  result->value.i = native->arguments->storage[0]->value.i;
  result->value.i %= native->arguments->storage[1]->value.i;

  return result;
}

