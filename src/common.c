#include "common.h"


void *
malloc0 (usize size)
{
  return calloc (1, size);
}


usize
round_pow2 (usize a)
{
  usize next = a + 1;

  next--;

  next |= next >> 1;
  next |= next >> 2;
  next |= next >> 4;
  next |= next >> 8;
  next |= next >> 16;

#if SIZE_MAX > 0xffffffff
  next |= next >> 32;
#endif

  next++;

  return next;
}


s64
min (s64 a, s64 b)
{
  return a < b ? a : b;
}


s64
max (s64 a, s64 b)
{
  return a > b ? a : b;
}

