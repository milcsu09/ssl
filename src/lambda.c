#include "lambda.h"
#include "program.h"
#include "scope.h"


struct lambda *
lambda_create (struct state *state, struct program *program, struct scope *scope)
{
  struct lambda *lambda;

  lambda = state_malloc (state, sizeof (struct lambda));

  state_unhealthy_return2 (state, NULL);

  lambda->program = program;

  lambda->scope = scope;

  return lambda;
}


void
lambda_destroy (struct state *state, struct lambda *lambda)
{
  state_free (state, lambda);
}


void
lambda_mark (struct lambda *lambda)
{
  program_mark (lambda->program);

  scope_mark (lambda->scope);
}


void
lambda_show (FILE *stream, struct lambda *lambda)
{
  UNUSED (lambda);

  fprintf (stream, "<lambda>");
}

