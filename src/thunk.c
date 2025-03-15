#include "ast.h"
#include "table.h"
#include "thunk.h"
#include <stdlib.h>

struct thunk *
thunk_create (struct ast *ast, struct table *table)
{
  struct thunk *thunk;

  thunk = calloc (1, sizeof (struct thunk));

  thunk->ast = ast_copy (ast, 0, NULL);
  thunk->table = table_copy (table);

  return thunk;
}

struct thunk *
thunk_copy (struct thunk *thunk)
{
  return thunk_create (thunk->ast, thunk->table);
}

void
thunk_destroy (struct thunk *thunk)
{
  ast_destroy (thunk->ast);
  table_destroy (thunk->table);
  free (thunk);
}

