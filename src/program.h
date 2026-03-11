#ifndef PROGRAM_H
#define PROGRAM_H


#include "error.h"
#include "insn.h"
#include "compiler.h"

#include <stdio.h>


struct code
{
  struct location location;

  insn i;
};


struct program
{
  struct
  {
    struct code *data;
    usize len;
    usize cap;
  } codes;

  struct
  {
    struct value **data;
    usize len;
    usize cap;
  } constants;

  struct
  {
    char **data;
    usize len;
    usize cap;
  } names;

  isize stack_size;
  isize stack_head;

  usize scope_size;
};


struct program *
program_create (struct state *state);

void
program_destroy (struct state *state, struct program *program);

void
program_mark (struct program *program);

usize
program_append_code (struct state *state, struct program *program, insn i,
                     struct location location);

usize
program_append_constant (struct state *state, struct program *program, struct value *constant);

usize
program_append_name (struct state *state, struct program *program, char *name);

usize
program_hole (struct state *state, struct program *program, usize size);

void
program_patch (struct program *program, usize index, insn i, struct location location);

void
program_show (FILE *, struct program *);

void
program_debug_print (struct program *, usize);


#endif // PROGRAM_H

