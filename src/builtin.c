#include "builtin.h"
#include "native.h"
#include "lambda.h"
#include "program.h"
#include "scope.h"
#include "string.h"
#include "list.h"
#include "value.h"

#include <math.h>
#include <string.h>


struct numeric_pair
{
  bool is_float;
  f64 a;
  f64 b;
};


struct numeric_pair
numeric_coerce (struct state *state, struct value *a, struct value *b)
{
  struct numeric_pair p = {0};

  if (a->kind == VALUE_INTEGER && b->kind == VALUE_INTEGER)
    {
      p.is_float = false;
      p.a = value_unbox_i (state, a);
      p.b = value_unbox_i (state, b);
      return p;
    }

  if (a->kind == VALUE_INTEGER && b->kind == VALUE_FLOAT)
    {
      p.is_float = true;
      p.a = value_unbox_i (state, a);
      p.b = value_unbox_f (state, b);
      return p;
    }

  if (a->kind == VALUE_FLOAT && b->kind == VALUE_INTEGER)
    {
      p.is_float = true;
      p.a = value_unbox_f (state, a);
      p.b = value_unbox_i (state, b);
      return p;
    }

  if (a->kind == VALUE_FLOAT && b->kind == VALUE_FLOAT)
    {
      p.is_float = true;
      p.a = value_unbox_f (state, a);
      p.b = value_unbox_f (state, b);
      return p;
    }

  return p;
}


struct value *
builtin_add (struct state *state, struct value **as)
{
  struct value *a = as[0];
  struct value *b = as[1];

  value_expect (state, a, 2, VALUE_INTEGER, VALUE_FLOAT);

  state_unhealthy_return2 (state, NULL);

  value_expect (state, b, 2, VALUE_INTEGER, VALUE_FLOAT);

  state_unhealthy_return2 (state, NULL);

  struct numeric_pair p = numeric_coerce (state, a, b);

  if (p.is_float)
    return value_box_f (state, p.a + p.b);

  return value_box_i (state, (s64)p.a + (s64)p.b);
}


struct value *
builtin_sub (struct state *state, struct value **as)
{

  struct value *a = as[0];
  struct value *b = as[1];

  value_expect (state, a, 2, VALUE_INTEGER, VALUE_FLOAT);

  state_unhealthy_return2 (state, NULL);

  value_expect (state, b, 2, VALUE_INTEGER, VALUE_FLOAT);

  state_unhealthy_return2 (state, NULL);

  struct numeric_pair p = numeric_coerce (state, a, b);

  if (p.is_float)
    return value_box_f (state, p.a - p.b);

  return value_box_i (state, (s64)p.a - (s64)p.b);
}


struct value *
builtin_mul (struct state *state, struct value **as)
{
  struct value *a = as[0];
  struct value *b = as[1];

  value_expect (state, a, 2, VALUE_INTEGER, VALUE_FLOAT);

  state_unhealthy_return2 (state, NULL);

  value_expect (state, b, 2, VALUE_INTEGER, VALUE_FLOAT);

  state_unhealthy_return2 (state, NULL);

  struct numeric_pair p = numeric_coerce (state, a, b);

  if (p.is_float)
    return value_box_f (state, p.a * p.b);

  return value_box_i (state, (s64)p.a * (s64)p.b);
}


struct value *
builtin_div (struct state *state, struct value **as)
{
  struct value *a = as[0];
  struct value *b = as[1];

  value_expect (state, a, 2, VALUE_INTEGER, VALUE_FLOAT);

  state_unhealthy_return2 (state, NULL);

  value_expect (state, b, 2, VALUE_INTEGER, VALUE_FLOAT);

  state_unhealthy_return2 (state, NULL);

  struct numeric_pair p = numeric_coerce (state, a, b);

  if (p.b == 0)
    {
      state_set_error (state, "arithmetic error");
      return NULL;
    }

  if (p.is_float)
    return value_box_f (state, p.a / p.b);

  return value_box_i (state, (s64)p.a / (s64)p.b);
}


struct value *
builtin_float (struct state *state, struct value **as)
{
  struct value *a = as[0];

  switch (a->kind)
    {
    case VALUE_INTEGER:
      return value_box_f (state, value_unbox_i (state, a));
    case VALUE_FLOAT:
      return a;
    default:
      state_set_error (state, "cannot convert to float");
      return NULL;
    }
}


struct value *
builtin_int (struct state *state, struct value **as)
{
  struct value *a = as[0];

  switch (a->kind)
    {
    case VALUE_INTEGER:
      return a;
    case VALUE_FLOAT:
      return value_box_i (state, value_unbox_f (state, a));
    default:
      state_set_error (state, "cannot convert to integer");
      return NULL;
    }
}


struct value *
builtin_mod (struct state *state, struct value **as)
{
  struct value *a = as[0];
  struct value *b = as[1];

  value_expect (state, a, 2, VALUE_INTEGER, VALUE_FLOAT);

  state_unhealthy_return2 (state, NULL);

  value_expect (state, b, 2, VALUE_INTEGER, VALUE_FLOAT);

  state_unhealthy_return2 (state, NULL);

  struct numeric_pair p = numeric_coerce (state, a, b);

  if (p.b == 0)
    {
      state_set_error (state, "arithmetic error");
      return NULL;
    }

  if (p.is_float)
    return value_box_f (state, fmod (p.a, p.b));

  return value_box_i (state, (s64)p.a % (s64)p.b);
}


struct value *
builtin_compare_eq (struct state *state, struct value **as)
{
  struct value *a = as[0];
  struct value *b = as[1];

  return value_box_i (state, value_compare_eq (a, b));
}


struct value *
builtin_compare_neq (struct state *state, struct value **as)
{
  struct value *a = as[0];
  struct value *b = as[1];

  return value_box_i (state, !value_compare_eq (a, b));
}


struct value *
builtin_compare_lt (struct state *state, struct value **as)
{
  struct value *a = as[0];
  struct value *b = as[1];

  value_expect (state, a, 2, VALUE_INTEGER, VALUE_FLOAT);

  state_unhealthy_return2 (state, NULL);

  value_expect (state, b, 2, VALUE_INTEGER, VALUE_FLOAT);

  state_unhealthy_return2 (state, NULL);

  struct numeric_pair p = numeric_coerce (state, a, b);

  return value_box_i (state, p.a < p.b);
}


struct value *
builtin_compare_gt (struct state *state, struct value **as)
{
  struct value *a = as[0];
  struct value *b = as[1];

  value_expect (state, a, 2, VALUE_INTEGER, VALUE_FLOAT);

  state_unhealthy_return2 (state, NULL);

  value_expect (state, b, 2, VALUE_INTEGER, VALUE_FLOAT);

  state_unhealthy_return2 (state, NULL);

  struct numeric_pair p = numeric_coerce (state, a, b);

  return value_box_i (state, p.a > p.b);
}


struct value *
builtin_compare_lte (struct state *state, struct value **as)
{
  struct value *a = as[0];
  struct value *b = as[1];

  value_expect (state, a, 2, VALUE_INTEGER, VALUE_FLOAT);

  state_unhealthy_return2 (state, NULL);

  value_expect (state, b, 2, VALUE_INTEGER, VALUE_FLOAT);

  state_unhealthy_return2 (state, NULL);

  struct numeric_pair p = numeric_coerce (state, a, b);

  return value_box_i (state, p.a <= p.b);
}


struct value *
builtin_compare_gte (struct state *state, struct value **as)
{
  struct value *a = as[0];
  struct value *b = as[1];

  value_expect (state, a, 2, VALUE_INTEGER, VALUE_FLOAT);

  state_unhealthy_return2 (state, NULL);

  value_expect (state, b, 2, VALUE_INTEGER, VALUE_FLOAT);

  state_unhealthy_return2 (state, NULL);

  struct numeric_pair p = numeric_coerce (state, a, b);

  return value_box_i (state, p.a >= p.b);
}


struct value *
builtin_head (struct state *state, struct value **as)
{
  struct list *li = value_unbox_li (state, as[0]);

  state_unhealthy_return2 (state, NULL);

  if (!li)
    {
      state_set_error (state, "`head []` is undefined");
      return NULL;
    }

  return li->head;
}


struct value *
builtin_tail (struct state *state, struct value **as)
{
  struct list *li = value_unbox_li (state, as[0]);

  state_unhealthy_return2 (state, NULL);

  if (!li)
    {
      state_set_error (state, "`tail []` is undefined");
      return NULL;
    }

  struct list *tail = li->tail;

  list_retain (tail);

  state_unhealthy_return2 (state, NULL);

  return value_box_li (state, tail);
}


struct value *
builtin_cons (struct state *state, struct value **as)
{
  struct value *a = as[0];

  struct list *li = value_unbox_li (state, as[1]);

  state_unhealthy_return2 (state, NULL);

  list_retain (li);

  struct list *tail = list_create (state, a, li);

  state_unhealthy_return2 (state, NULL);

  return value_box_li (state, tail);
}


struct value *
builtin_print (struct state *state, struct value **as)
{
  struct value *a = as[0];

  switch (a->kind)
    {
    case VALUE_STRING:
      printf ("%s", a->data.s);
      break;

    default:
      value_show (stdout, a);
      break;
    }

  return value_box_u (state);
}


struct value *
builtin_printf (struct state *state, struct value **as)
{
  char *format_s = value_unbox_s (state, as[0]);

  state_unhealthy_return2 (state, NULL);

  struct list *format_li = value_unbox_li (state, as[1]);

  state_unhealthy_return2 (state, NULL);

  struct list *l = format_li;

  for (char *p = format_s; *p; ++p)
    {
      if (p[0] == '\\' && p[1] == '_')
        {
          putchar (*++p);
          continue;
        }

      if (*p != '_')
        {
          putchar (*p);
          continue;
        }

      if (l)
        {
          switch (l->head->kind)
            {
            case VALUE_STRING:
              printf ("%s", l->head->data.s);
              break;

            default:
              value_show (stdout, l->head);
              break;
            }

          l = l->tail;
        }
      else
        {
          putchar('_');
        }
    }

  return value_box_u (state);
}


struct value *
builtin_printl (struct state *state, struct value **as)
{
  struct value *unit = builtin_print (state, as);

  state_unhealthy_return2 (state, NULL);

  putchar ('\n');

  return unit;
}


struct value *
builtin_printlf (struct state *state, struct value **as)
{
  struct value *unit = builtin_printf (state, as);

  state_unhealthy_return2 (state, NULL);

  putchar ('\n');

  return unit;
}


struct value *
builtin_input (struct state *state, struct value **as)
{
  char *s = value_unbox_s (state, as[0]);

  state_unhealthy_return2 (state, NULL);

  printf ("%s", s);

  char buffer[1024];

  if (fgets (buffer, sizeof buffer, stdin) == NULL)
    {
      state_set_error (state, "failed to read input");

      return NULL;
    }

  buffer[strcspn (buffer, "\n")] = '\0';

  char *buffer_copy = c_string_copy (state, buffer, false);

  state_unhealthy_return2 (state, NULL);

  return value_box_s (state, buffer_copy);
}


struct value *
builtin_reverseString (struct state *state, struct value **as)
{
  char *s = value_unbox_s (state, as[0]);

  state_unhealthy_return2 (state, NULL);

  size_t len = strlen (s);

  char *str = state_malloc (state, len + 1);

  state_unhealthy_return2 (state, NULL);

  strcpy (str, s);

  for (size_t i = 0; i < len / 2; ++i)
    {
      char temp = str[i];
      str[i] = str[len - i - 1];
      str[len - i - 1] = temp;
    }

  return value_box_s (state, str);
}


struct value *
builtin_stringToInteger (struct state *state, struct value **as)
{
  char *s = value_unbox_s (state, as[0]);

  state_unhealthy_return2 (state, NULL);

  return value_box_i (state, strtoll (s, NULL, 10));
}


struct value *
builtin_stringToFloat (struct state *state, struct value **as)
{
  char *s = value_unbox_s (state, as[0]);

  state_unhealthy_return2 (state, NULL);

  return value_box_f (state, strtof (s, NULL));
}


struct value *
builtin_error (struct state *state, struct value **as)
{
  char *s = value_unbox_s (state, as[0]);

  state_unhealthy_return2 (state, NULL);

  state_set_error (state, "%s", s);

  return NULL;
}


struct value *
builtin_dis (struct state *state, struct value **as)
{
  struct lambda *la = value_unbox_la (state, as[0]);

  state_unhealthy_return2 (state, NULL);

  fprintf (stderr, "\n");

  program_debug_print (la->program, 2);

  fprintf (stderr, "\n");
  fprintf (stderr, "\n");

  return value_box_u (state);
}


struct value *
builtin_randbool (struct state *state, struct value **as)
{
  UNUSED (as);

  return value_box_i (state, rand () % 2);
}


struct value *
builtin_typeid (struct state *state, struct value **as)
{
  return value_box_i (state, as[0]->kind);
}


////////////////////////////////////////////////////////////////////////////////////////////////////


void
native_set (struct state *state, struct scope *scope, char *key,
            native_function *f, usize remaining)
{
  struct native *native;

  native = native_create (state, f, remaining);

  struct value *value;

  value = value_box_n (state, native);

  scope_set (scope, key, value);
}


void
builtin_set (struct state *state, struct scope *scope)
{
  scope_set (scope, "false",     value_box_i (state, 0));
  scope_set (scope, "true",      value_box_i (state, 1));
  scope_set (scope, "otherwise", value_box_i (state, 1));

  scope_set (scope, "unitid",     value_box_i (state, VALUE_UNIT));
  scope_set (scope, "integerid",  value_box_i (state, VALUE_INTEGER));
  scope_set (scope, "floatid",    value_box_i (state, VALUE_FLOAT));
  scope_set (scope, "stringid",   value_box_i (state, VALUE_STRING));
  scope_set (scope, "listid",     value_box_i (state, VALUE_LIST));
  scope_set (scope, "lambdaid",   value_box_i (state, VALUE_LAMBDA));
  scope_set (scope, "nativeid",   value_box_i (state, VALUE_NATIVE));

  native_set (state, scope, "+", builtin_add, 2);
  native_set (state, scope, "-", builtin_sub, 2);
  native_set (state, scope, "*", builtin_mul, 2);
  native_set (state, scope, "/", builtin_div, 2);
  native_set (state, scope, "%", builtin_mod, 2);

  native_set (state, scope, "float", builtin_float, 1);
  native_set (state, scope, "int",   builtin_int,   1);

  native_set (state, scope, "==", builtin_compare_eq, 2);
  native_set (state, scope, "/=", builtin_compare_neq, 2);
  native_set (state, scope, "<",  builtin_compare_lt, 2);
  native_set (state, scope, ">",  builtin_compare_gt, 2);
  native_set (state, scope, "<=", builtin_compare_lte, 2);
  native_set (state, scope, ">=", builtin_compare_gte, 2);

  native_set (state, scope, "head", builtin_head, 1);
  native_set (state, scope, "tail", builtin_tail, 1);
  native_set (state, scope, ":",    builtin_cons, 2);

  native_set (state, scope, "print",   builtin_print,   1);
  native_set (state, scope, "printf",  builtin_printf,  2);
  native_set (state, scope, "printl",  builtin_printl,  1);
  native_set (state, scope, "printlf", builtin_printlf, 2);

  native_set (state, scope, "input", builtin_input, 1);
  native_set (state, scope, "reverseString", builtin_reverseString, 1);

  native_set (state, scope, "stringToInteger", builtin_stringToInteger, 1);
  native_set (state, scope, "stringToFloat",   builtin_stringToFloat,   1);

  native_set (state, scope, "error", builtin_error, 1);

  native_set (state, scope, "dis", builtin_dis, 1);

  native_set (state, scope, "randbool", builtin_randbool, 1);

  native_set (state, scope, "typeid",   builtin_typeid, 1);
}

