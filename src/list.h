#ifndef LIST_H
#define LIST_H


#include "common.h"
#include "state.h"

#include <stdbool.h>
#include <stdio.h>


struct value;


struct list
{
  struct value *head;

  struct list *tail;

  // Prevents SELF and TAIL from being freed, under assumption TAIL.references >= SELF.references.
  usize references;

  // Optimization trick. During mark()-ing, if CURRENT.marked, assume CURRENT.TAIL.marked.
  bool marked;
};


struct list *
list_create (struct state *state, struct value *head, struct list *tail);

void
list_retain (struct list *list);

void
list_destroy (struct state *state, struct list *list);

void
list_mark (struct list *list);

void
list_unmark (struct list *list);

void
list_show (FILE *stream, struct list *list);

usize
list_length (struct list *list);

bool
list_compare_eq (struct list *a, struct list *b);


#endif // LIST_H

