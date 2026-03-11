#ifndef COMMON_H
#define COMMON_H


#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>


#define UNUSED(a) (void)(a)


typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef size_t usize;
typedef ptrdiff_t isize;

typedef float f32;
typedef double f64;


#define DA_DEFAULT 8


#define da_reserve(state, da, expected)                                                            \
  do                                                                                               \
    {                                                                                              \
      if ((expected) > (da)->cap)                                                                  \
        {                                                                                          \
          if ((da)->cap == 0)                                                                      \
            (da)->cap = DA_DEFAULT;                                                                \
                                                                                                   \
          while ((expected) > (da)->cap)                                                           \
            (da)->cap *= 2;                                                                        \
                                                                                                   \
          (da)->data = state_realloc ((state), (da)->data, (da)->cap * sizeof *(da)->data);        \
        }                                                                                          \
    }                                                                                              \
  while (0)


#define da_append(state, da, item)                                                                 \
  do                                                                                               \
    {                                                                                              \
      da_reserve ((state), (da), (da)->len + 1);                                                   \
                                                                                                   \
      if (!(state)->unhealthy)                                                                     \
        (da)->data[(da)->len++] = item;                                                            \
    }                                                                                              \
  while (0)


#define da_clear(state, da)                                                                        \
  do                                                                                               \
    {                                                                                              \
      state_free ((state), (da)->data);                                                            \
                                                                                                   \
      (da)->data = NULL;                                                                           \
      (da)->len = 0;                                                                               \
      (da)->cap = 0;                                                                               \
    }                                                                                              \
  while (0)


void *
malloc0 (usize size);

usize
round_pow2 (usize a);

s64
min (s64 a, s64 b);

s64
max (s64 a, s64 b);


#endif // COMMON_H

