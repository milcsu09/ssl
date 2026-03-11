#ifndef VALUE_H
#define VALUE_H


#include "common.h"
#include "state.h"

#include <stdbool.h>
#include <stdio.h>


struct lambda;
struct list;
struct native;
struct program;


union value_data
{
  s64 i;

  f64 f;

  char *s;

  struct program *p;

  struct list *li;

  struct lambda *la;

  struct native *n;
};


enum value_kind
{
  VALUE_UNIT,

  VALUE_INTEGER,

  VALUE_FLOAT,

  VALUE_STRING,

  VALUE_LIST,

  VALUE_LAMBDA,

  VALUE_NATIVE,

  // Miscellaneous
  VALUE_PROGRAM,
};


// NOTE: No out-of-bounds check!
const char *
value_kind_string (enum value_kind kind);


struct value
{
  struct value *next;

  union value_data data;

  enum value_kind kind;

  bool marked;
};


struct value *
value_create (struct state *state, enum value_kind kind);

struct value *
value_box_u (struct state *state);

struct value *
value_box_i (struct state *state, s64 i);

struct value *
value_box_f (struct state *state, f64 f);

struct value *
value_box_s (struct state *state, char *s);

struct value *
value_box_li (struct state *state, struct list *li);

struct value *
value_box_la (struct state *state, struct lambda *la);

struct value *
value_box_n (struct state *state, struct native *n);

struct value *
value_box_p (struct state *state, struct program *p);

void
value_destroy (struct state *state, struct value *value);

void
value_mark (struct value *value);

void
value_unmark (struct value *value);

void
value_expect (struct state *state, struct value *value, usize n, ...);

void
value_unbox_u (struct state *state, struct value *value);

s64
value_unbox_i (struct state *state, struct value *value);

f64
value_unbox_f (struct state *state, struct value *value);

char *
value_unbox_s (struct state *state, struct value *value);

struct list *
value_unbox_li (struct state *state, struct value *value);

struct lambda *
value_unbox_la (struct state *state, struct value *value);

struct native *
value_unbox_n (struct state *state, struct value *value);

void
value_show (FILE *stream, struct value *value);

bool
value_compare_eq (struct value *a, struct value *b);

bool
value_bool (struct value *value);


#endif // VALUE_H

