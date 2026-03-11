#ifndef LAMBDA_H
#define LAMBDA_H


#include "state.h"

#include <stdio.h>


struct program;
struct scope;


struct lambda
{
  // NOTE: Program's lifetime is NOT handled by 'struct lambda'.
  struct program *program;

  struct scope *scope;
};


struct lambda *
lambda_create (struct state *state, struct program *program, struct scope *scope);

void
lambda_destroy (struct state *state, struct lambda *lambda);

void
lambda_mark (struct lambda *lambda);

void
lambda_show (FILE *stream, struct lambda *lambda);


#endif // LAMBDA_H

