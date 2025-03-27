#include "array.h"
#include "ast.h"
#include "evaluator.h"
#include "function.h"
#include "standard.h"
#include "native.h"
#include "table.h"
#include "value.h"
#include <stdio.h>

struct value *
standard_invoke (struct value *f, struct value *argument)
{
  if (value_match_error (f))
    return f;

  if (value_match (f, VALUE_FUNCTION))
    {
      struct function *function = f->value.function;

      struct table *f_table = table_copy (function->table);
      f_table->parent = function->table;

      const char *name = function->ast->child->value.token.value.s;
      table_append (f_table, table_entry_create (name, argument));

      struct value *result = evaluate (function->ast->child->next, f_table);

      table_destroy (f_table);

      // value_retain (result);
      return result;
    }

  if (value_match (f, VALUE_NATIVE))
    {
      struct value *native_v = value_copy (f);

      native_v->location = f->location;

      struct native *native = native_v->value.native;

      native_c_function_t c_function = native->function;
      struct array *arguments = array_push_back (native->arguments, argument);

      array_destroy (native->arguments);
      native->arguments = arguments;

      struct value *result = c_function (native_v);

      if (result != native_v)
        value_destroy (native_v);

      return result;
    }

  struct error e = error_create ("attempted to call %s",
                                 value_type_string (f->type));

  return value_create_e (e, f->location);
}

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
    printf ("%s", native->arguments->storage[0]->value.s);
  else
    {
      value_debug_print (native->arguments->storage[0]);
      // printf ("\n");
    }

  return value_create (VALUE_NOTHING, curry->location);
}

struct value *
standard_f_printl (struct value *curry)
{
  /* single-argument */
  struct value *result = standard_f_print (curry);
  printf ("\n");
  return result;
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

struct value *
standard_f_range (struct value *curry)
{
  struct native *native = curry->value.native;
  /* single-argument */

  size_t n = native->arguments->storage[0]->value.i;

  struct value **storage = calloc (n, sizeof (struct value *));

  for (size_t i = 0; i < n; ++i)
    {
      struct value *index;

      index = value_create (VALUE_INTEGER, curry->location);
      index->value.i = i;

      storage[i] = index;
    }

  struct value *value;

  value = value_create (VALUE_ARRAY, curry->location);
  value->value.array = array_create (storage, n);

  for (size_t i = 0; i < n; ++i)
    value_destroy (storage[i]);
  free (storage);

  return value;
}

struct value *
standard_f_map (struct value *curry)
{
  struct native *native = curry->value.native;
  if (native->arguments->size < 2)
    return curry;

  struct array *array = native->arguments->storage[1]->value.array;
  size_t n = array->size;

  struct value **storage = calloc (n, sizeof (struct value *));

  for (size_t i = 0; i < n; ++i)
    {
      struct value *ret;

      ret = standard_invoke (native->arguments->storage[0], array->storage[i]);
      if (value_match_error (ret))
        {
          for (size_t j = 0; j < i; ++i)
            value_destroy (storage[j]);
          free (storage);
          return ret;
        }

      storage[i] = ret;
    }

  struct value *value;

  value = value_create (VALUE_ARRAY, curry->location);
  value->value.array = array_create (storage, n);

  for (size_t i = 0; i < n; ++i)
    value_destroy (storage[i]);
  free (storage);

  return value;
}

struct value *
standard_f_map_ (struct value *curry)
{
  struct native *native = curry->value.native;
  if (native->arguments->size < 2)
    return curry;

  struct array *array = native->arguments->storage[1]->value.array;

  size_t n = array->size;

  for (size_t i = 0; i < n; ++i)
    {
      struct value *ret;

      ret = standard_invoke (native->arguments->storage[0], array->storage[i]);
      if (value_match_error (ret))
        return ret;

      value_destroy (ret);
    }

  return value_create (VALUE_NOTHING, curry->location);
}

struct value *
standard_f_push_front (struct value *curry)
{
  struct native *native = curry->value.native;
  if (native->arguments->size < 2)
    return curry;

  struct value *value = native->arguments->storage[0];
  struct array *array = native->arguments->storage[1]->value.array;
  struct array *new = array_push_front (array, value);

  struct value *result;

  result = value_create (VALUE_ARRAY, curry->location);

  result->value.array = new;

  return result;
}

struct value *
standard_f_error (struct value *curry)
{
  struct native *native = curry->value.native;
  /* single-argument */

  struct error e = error_create (native->arguments->storage[0]->value.s);
  return value_create_e (e, curry->location);
}

