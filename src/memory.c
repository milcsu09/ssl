#include "memory.h"
#include "state.h"

#include <stdint.h>
#include <stdio.h>


#define AA_CHUNK_SIZE (0x1000)


struct aa_chunk
{
  struct aa_chunk *next;

  u8 *head;
  u8 *tail;

  u8 data[];
};


struct aa_chunk *
aa_chunk_create (struct state *state, usize n_bytes)
{
  if (n_bytes < AA_CHUNK_SIZE)
    n_bytes = AA_CHUNK_SIZE;

  usize a_bytes = sizeof (struct aa_chunk) + n_bytes + ALIGN_MAX - 1;

  struct aa_chunk *chunk;

  chunk = state_malloc (state, a_bytes);

  state_unhealthy_return2 (state, NULL);

  chunk->next = NULL;

  chunk->head = chunk->data;
  chunk->tail = chunk->data + n_bytes;

  return chunk;
}


struct aa
{
  struct aa_chunk *head;
  struct aa_chunk *tail;
};


struct aa *
aa_create (struct state *state)
{
  struct aa *aa;

  aa = state_malloc (state, sizeof (struct aa));

  state_unhealthy_return2 (state, NULL);

  aa->head = NULL;
  aa->tail = NULL;

  return aa;
}


void
aa_destroy (struct state *state, struct aa *aa)
{
  struct aa_chunk *chunk = aa->head;

  while (chunk)
    {
      struct aa_chunk *next = chunk->next;

      state_free (state, chunk);

      chunk = next;
    }

  state_free (state, aa);
}


void *
align_forward_p (void *p, usize a)
{
  uintptr_t x = (uintptr_t)p;
  uintptr_t m = (a - 1);
  uintptr_t y = (x + m) & ~m;
  return (void *)y;
}


void *
aa_malloc (struct state *state, struct aa *aa, usize size)
{
  struct aa_chunk *chunk = aa->tail;

  if (chunk == NULL)
    {
      chunk = aa_chunk_create (state, size);

      state_unhealthy_return2 (state, NULL);

      aa->head = chunk;
      aa->tail = chunk;
    }

  u8 *head = align_forward_p (chunk->head, ALIGN_MAX);
  u8 *next = head + size;

  if (next > chunk->tail)
    {
      chunk = aa_chunk_create (state, size);

      state_unhealthy_return2 (state, NULL);

      aa->tail = aa->tail->next = chunk;

      head = align_forward_p (chunk->head, ALIGN_MAX);
      next = head + size;
    }

  chunk->head = next;

  return head;
}

