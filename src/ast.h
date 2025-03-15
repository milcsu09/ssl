#ifndef AST_H
#define AST_H

#include "arena.h"
#include "token.h"

union ast_entry
{
  struct token token;
  struct error error;
};

enum ast_type
{
  AST_ERROR,
  AST_IDENTIFIER,
  AST_STRING,
  AST_INTEGER,
  AST_FLOAT,
  AST_ARRAY,
  AST_VARIABLE_ASSIGNMENT,
  AST_FUNCTION_DEFINITION,
  AST_FUNCTION_INVOCATION,
  AST_TURNARY,
  AST_WHERE,
  AST_PROGRAM,
};

struct ast
{
  union ast_entry value;
  enum ast_type type;
  struct location location;
  struct ast *child;
  struct ast *next;
};

const char *ast_type_string (enum ast_type);

struct ast *ast_create (enum ast_type, struct location, struct arena *);
struct ast *ast_create_e (struct error, struct location, struct arena *);
struct ast *ast_copy (struct ast *, int, struct arena *);
void ast_append (struct ast *, struct ast *);
void ast_attach (struct ast *, struct ast *);
int ast_match (struct ast *, enum ast_type);
int ast_match_error (struct ast *);
void ast_debug_print (struct ast *, size_t);

#endif // AST_H

