#ifndef MEMORY_H
#define MEMORY_H


#include "common.h"


struct state;


union align_max
{
  long l;
  long long ll;

  double d;
  long double ld;

  void *p;
  void (*fp) (void);
};


struct align_max_offset
{
  char c; // NOTE: Dummy field.

  union align_max align_max;
};


enum
{
  ALIGN_MAX = offsetof (struct align_max_offset, align_max)
};


struct aa;


struct aa *
aa_create (struct state *state);

void
aa_destroy (struct state *state, struct aa *aa);

void *
aa_malloc (struct state *state, struct aa *aa, usize size);


#endif // MEMORY_H

