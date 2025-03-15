#include "ast.h"
#include "table.h"
#include "function.h"
#include <stdlib.h>

struct function *
function_create (struct ast *ast, struct table *table)
{
  struct function *function;

  function = calloc (1, sizeof (struct function));

  function->ast = ast_copy (ast, 0, NULL);
  function->table = table_copy (table);

  return function;
}

struct function *
function_copy (struct function *function)
{
  return function_create (function->ast, function->table);
}

void
function_destroy (struct function *function)
{
  ast_destroy (function->ast);
  table_destroy (function->table);
  free (function);
}

