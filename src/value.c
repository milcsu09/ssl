#include "lambda.h"
#include "list.h"
#include "native.h"
#include "program.h"
#include "value.h"

#include <stdarg.h>
#include <string.h>


static const char *const VALUE_KIND_STRING[] = {
  "()",
  "integer",
  "float",
  "string",
  "list",
  "lambda",
  "native",
  "program",
};


const char *
value_kind_string (enum value_kind kind)
{
  return VALUE_KIND_STRING[kind];
}


struct value *
value_create (struct state *state, enum value_kind kind)
{
  struct value *value;

  value = state_malloc (state, sizeof (struct value));

  state_unhealthy_return2 (state, NULL);

  value->kind = kind;

  state_register_value (state, value);

  return value;
}


struct value *
value_box_u (struct state *state)
{
  return value_create (state, VALUE_UNIT);
}


struct value *
value_box_i (struct state *state, s64 i)
{
  struct value *value;

  value = value_create (state, VALUE_INTEGER);

  state_unhealthy_return2 (state, NULL);

  value->data.i = i;

  return value;
}


struct value *
value_box_f (struct state *state, f64 f)
{
  struct value *value;

  value = value_create (state, VALUE_FLOAT);

  state_unhealthy_return2 (state, NULL);

  value->data.f = f;

  return value;
}


struct value *
value_box_s (struct state *state, char *s)
{
  struct value *value;

  value = value_create (state, VALUE_STRING);

  state_unhealthy_return2 (state, NULL);

  value->data.s = s;

  return value;
}


struct value *
value_box_li (struct state *state, struct list *li)
{
  struct value *value;

  value = value_create (state, VALUE_LIST);

  state_unhealthy_return2 (state, NULL);

  value->data.li = li;

  return value;
}


struct value *
value_box_la (struct state *state, struct lambda *la)
{
  struct value *value;

  value = value_create (state, VALUE_LAMBDA);

  state_unhealthy_return2 (state, NULL);

  value->data.la = la;

  return value;
}


struct value *
value_box_n (struct state *state, struct native *n)
{
  struct value *value;

  value = value_create (state, VALUE_NATIVE);

  state_unhealthy_return2 (state, NULL);

  value->data.n = n;

  return value;
}


struct value *
value_box_p (struct state *state, struct program *p)
{
  struct value *value;

  value = value_create (state, VALUE_PROGRAM);

  state_unhealthy_return2 (state, NULL);

  value->data.p = p;

  return value;
}


void
value_destroy (struct state *state, struct value *value)
{
  switch (value->kind)
    {
    case VALUE_STRING:
      state_free (state, value->data.s);
      break;
    case VALUE_LIST:
      list_destroy (state, value->data.li);
      break;
    case VALUE_LAMBDA:
      lambda_destroy (state, value->data.la);
      break;
    case VALUE_NATIVE:
      native_destroy (state, value->data.n);
      break;
    case VALUE_PROGRAM:
      program_destroy (state, value->data.p);
      break;
    default:
      break;
    }

  state_free (state, value);
}


void
value_mark (struct value *value)
{
  if (value->marked)
    return;

  value->marked = true;

  switch (value->kind)
    {
    case VALUE_LIST:
      list_mark (value->data.li);
      break;
    case VALUE_LAMBDA:
      lambda_mark (value->data.la);
      break;
    case VALUE_NATIVE:
      native_mark (value->data.n);
      break;
    case VALUE_PROGRAM:
      program_mark (value->data.p);
      break;
    default:
      break;
    }
}


void
value_unmark (struct value *value)
{
  value->marked = false;

  switch (value->kind)
    {
    case VALUE_LIST:
      list_unmark (value->data.li);
      break;
    default:
      break;
    }
}


void
value_expect (struct state *state, struct value *value, usize n, ...)
{
  if (n == 0)
    return;

  va_list va;

  va_start (va, n);

  enum value_kind kind;

  for (usize i = 0; i < n; i++)
    {
      kind = (enum value_kind) va_arg (va, int);

      if (value->kind == kind)
        {
          va_end (va);
          return;
        }
    }

  va_end (va);

  if (n == 1)
    {
      const char *a = value_kind_string (kind);
      const char *b = value_kind_string (value->kind);
      state_set_error (state, "unexpected %s, expected %s", b, a);
    }
  else
    {
      const char *b = value_kind_string (value->kind);
      state_set_error (state, "unexpected %s", b);
    }
}


void
value_unbox_u (struct state *state, struct value *value)
{
  value_expect (state, value, 1, VALUE_UNIT);
}


s64
value_unbox_i (struct state *state, struct value *value)
{
  value_expect (state, value, 1, VALUE_INTEGER);

  state_unhealthy_return2 (state, 0);

  return value->data.i;
}


f64
value_unbox_f (struct state *state, struct value *value)
{
  value_expect (state, value, 1, VALUE_FLOAT);

  state_unhealthy_return2 (state, 0.0);

  return value->data.f;
}


char *
value_unbox_s (struct state *state, struct value *value)
{
  value_expect (state, value, 1, VALUE_STRING);

  state_unhealthy_return2 (state, NULL);

  return value->data.s;
}


struct list *
value_unbox_li (struct state *state, struct value *value)
{
  value_expect (state, value, 1, VALUE_LIST);

  state_unhealthy_return2 (state, NULL);

  return value->data.li;
}


struct lambda *
value_unbox_la (struct state *state, struct value *value)
{
  value_expect (state, value, 1, VALUE_LAMBDA);

  state_unhealthy_return2 (state, NULL);

  return value->data.la;
}


struct native *
value_unbox_n (struct state *state, struct value *value)
{
  value_expect (state, value, 1, VALUE_NATIVE);

  state_unhealthy_return2 (state, NULL);

  return value->data.n;
}


void
value_show (FILE *stream, struct value *value)
{
  switch (value->kind)
    {
    case VALUE_UNIT:
      fprintf (stream, "()");
      break;
    case VALUE_INTEGER:
      fprintf (stream, "%ld", value->data.i);
      break;
    case VALUE_FLOAT:
      fprintf (stream, "%g", value->data.f);
      break;
    case VALUE_STRING:
      fprintf (stream, "\"%s\"", value->data.s);
      break;
    case VALUE_LIST:
      list_show (stream, value->data.li);
      break;
    case VALUE_LAMBDA:
      lambda_show (stream, value->data.la);
      break;
    case VALUE_NATIVE:
      native_show (stream, value->data.n);
      break;
    case VALUE_PROGRAM:
      program_show (stream, value->data.p);
      break;
    default:
      break;
    }
}


bool
value_compare_eq (struct value *a, struct value *b)
{
  if (a->kind != b->kind)
    return false;

  switch (a->kind)
    {
    case VALUE_UNIT:
      return true;
    case VALUE_INTEGER:
      return a->data.i == b->data.i;
    case VALUE_FLOAT:
      return a->data.f == b->data.f;
    case VALUE_STRING:
      return strcmp (a->data.s, b->data.s) == 0;
    case VALUE_LIST:
      return list_compare_eq (a->data.li, b->data.li);
    default:
      // NOTE: Compare pointers.
      return a == b;
    }
}


bool
value_bool (struct value *value)
{
  switch (value->kind)
    {
    case VALUE_UNIT:
      return false;
    case VALUE_INTEGER:
      return value->data.i != 0;
    case VALUE_FLOAT:
      return value->data.f != 0;
    case VALUE_STRING:
      return value->data.s[0] != 0;
    case VALUE_LIST:
      return value->data.li != NULL;
    case VALUE_LAMBDA:
      return true;
    case VALUE_NATIVE:
      return true;
    default:
      return false;
    }
}

