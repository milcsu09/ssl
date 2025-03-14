#include "ast.h"
#include <stdio.h>
#include <stdlib.h>

static const char *const AST_TYPE_STRING[] = {
  "error",
  "identifier",
  "string",
  "integer",
  "float",
  "array",
  "variable-assignment",
  "function-definition",
  "function-invocation",
  "turnary",
  "where",
  "program",
};

const char *
ast_type_string (enum ast_type type)
{
  return AST_TYPE_STRING[type];
}

struct ast *
ast_create (enum ast_type type, struct location location)
{
  struct ast *ast;

  ast = calloc (1, sizeof (struct ast));

  ast->type = type;
  ast->location = location;

  return ast;
}

struct ast *
ast_create_e (struct error e, struct location location)
{
  struct ast *ast;

  ast = ast_create (AST_ERROR, location);

  ast->value.error = e;

  return ast;
}

struct ast *
ast_copy (struct ast *ast, int next)
{
  if (ast == NULL)
    return NULL;

  struct ast *copy;

  copy = ast_create (ast->type, ast->location);

  switch (ast->type)
    {
    case AST_ERROR:
      copy->value.error = ast->value.error;
      break;
    default:
      copy->value.token = token_copy (ast->value.token);
      break;
    }

  copy->child = ast_copy (ast->child, 1);

  if (next)
    copy->next = ast_copy (ast->next, 1);

  return copy;
}

void
ast_destroy (struct ast *ast)
{
  if (ast == NULL)
    return;

  ast_destroy (ast->child);
  ast_destroy (ast->next);

  switch (ast->type)
    {
    case AST_ERROR:
      break;
    default:
      token_destroy (ast->value.token);
      break;
    }

  free (ast);
}

void
ast_append (struct ast *ast, struct ast *node)
{
  if (ast->child == NULL)
    ast->child = node;
  else
    ast_attach (ast->child, node);
}

void
ast_attach (struct ast *ast, struct ast *node)
{
  struct ast *current = ast;

  while (current->next != NULL)
    current = current->next;

  current->next = node;
}

int
ast_match (struct ast *ast, enum ast_type type)
{
  return ast->type == type;
}

int
ast_match_error (struct ast *ast)
{
  return ast_match (ast, AST_ERROR);
}

#define AST_DEBUG_INDENT 4

void
ast_debug_print (struct ast *ast, size_t depth)
{
  if (ast == NULL)
    return;

  for (size_t i = depth * AST_DEBUG_INDENT; i--;)
    {
      size_t line = (i % AST_DEBUG_INDENT == (AST_DEBUG_INDENT - 1));
      if (line)
        printf ("┊");
      else
        printf (" ");
    }

  printf ("%s: ", ast_type_string (ast->type));

  switch (ast->type)
    {
    case AST_ERROR:
      printf ("%s", ast->value.error.message);
      printf (" ");
      break;
    default:
      if (ast->value.token.type != TOKEN_NOTHING)
        {
          token_debug_print (ast->value.token);
          printf (" ");
        }
      break;
    }

  printf ("(");
  location_debug_print (ast->location);
  printf (")\n");

  ast_debug_print (ast->child, depth + 1);
  ast_debug_print (ast->next, depth);
}

