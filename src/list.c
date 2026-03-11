#include "list.h"
#include "value.h"
#include <stdbool.h>


struct list *
list_create (struct state *state, struct value *head, struct list *tail)
{
  struct list *list;

  list = state_malloc (state, sizeof (struct list));

  state_unhealthy_return2 (state, NULL);

  list->head = head;
  list->tail = tail;

  list->references = 1;

  return list;
}


void
list_retain (struct list *list)
{
  if (!list)
    return;

  list->references++;
}


void
list_destroy (struct state *state, struct list *list)
{
  while (list)
    {
      if (--list->references > 0)
        return;

      struct list *next = list->tail;

      state_free (state, list);

      list = next;
    }
}


void
list_mark (struct list *list)
{
  for (struct list *p = list; p && !p->marked; p = p->tail)
    {
      value_mark (p->head);

      p->marked = true;
    }
}


void
list_unmark (struct list *list)
{
  for (struct list *p = list; p && p->marked; p = p->tail)
    p->marked = false;
}


void
list_show (FILE *stream, struct list *list)
{
  fprintf (stream, "[");

  for (struct list *p = list; p; p = p->tail)
    {
      value_show (stream, p->head);

      if (p->tail)
        fprintf (stream, ",");
    }

  fprintf (stream, "]");
}


usize
list_length (struct list *list)
{
  usize n;

  for (n = 0; list; list = list->tail)
    n++;

  return n;
}


bool
list_compare_eq (struct list *a, struct list *b)
{
  while (a && b)
    {
      if (!value_compare_eq (a->head, b->head))
        return false;

      a = a->tail;
      b = b->tail;
    }

  return a == NULL && b == NULL;
}

