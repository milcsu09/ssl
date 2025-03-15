#include "ast.h"
#include "evaluator.h"
#include "function.h"
#include "string.h"
#include "table.h"
#include "value.h"
#include <stdio.h>
#include <stdlib.h>

// struct value *evaluate_error (struct ast *, struct table *);
struct value *evaluate_identifier (struct ast *, struct table *);
struct value *evaluate_string (struct ast *, struct table *);
struct value *evaluate_integer (struct ast *, struct table *);
struct value *evaluate_float (struct ast *, struct table *);
struct value *evaluate_array (struct ast *, struct table *);
struct value *evaluate_variable_assignment (struct ast *, struct table *);
struct value *evaluate_function_definition (struct ast *, struct table *);
struct value *evaluate_function_invocation (struct ast *, struct table *);
// struct value *evaluate_turnary (struct ast *, struct table *);
// struct value *evaluate_where (struct ast *, struct table *);
struct value *evaluate_program (struct ast *, struct table *);

struct value *
evaluate (struct ast *ast, struct table *table)
{
  switch (ast->type)
    {
    case AST_ERROR:
      printf ("evaluate (): AST has ERROR type ???\n");
      abort ();
      // return value_create_e (ast->value.error, ast->location);
    case AST_IDENTIFIER:
      return evaluate_identifier (ast, table);
    case AST_STRING:
      return evaluate_string (ast, table);
    case AST_INTEGER:
      return evaluate_integer (ast, table);
    case AST_FLOAT:
      return evaluate_float (ast, table);
    case AST_ARRAY:
      return evaluate_array (ast, table);
    case AST_VARIABLE_ASSIGNMENT:
      return evaluate_variable_assignment (ast, table);
    case AST_FUNCTION_DEFINITION:
      return evaluate_function_definition (ast, table);
    case AST_FUNCTION_INVOCATION:
      return evaluate_function_invocation (ast, table);
    // case AST_TURNARY:
    //   break;
    // case AST_WHERE:
    //   break;
    case AST_PROGRAM:
      return evaluate_program (ast, table);
    default:
      printf ("evaluate (): unable to evaluate %s node\n",
              ast_type_string (ast->type));
      abort ();
    }

  return NULL;
}

struct value *
evaluate_identifier (struct ast *ast, struct table *table)
{
  struct value *value;

  const char *name = ast->value.token.value.s;

  value = table_find (table, name);
  if (value == NULL)
    return value_create_e (error_create ("variable not in scope: %s", name),
                           ast->location);

  return value_copy (value);
}

struct value *
evaluate_string (struct ast *ast, struct table *table)
{
  (void)table;

  struct value *value;

  value = value_create (VALUE_STRING, ast->location);

  value->value.s = string_copy (ast->value.token.value.s, NULL);

  return value;
}

struct value *
evaluate_integer (struct ast *ast, struct table *table)
{
  (void)table;

  struct value *value;

  value = value_create (VALUE_INTEGER, ast->location);

  value->value.i = ast->value.token.value.i;

  return value;
}

struct value *
evaluate_float (struct ast *ast, struct table *table)
{
  (void)table;

  struct value *value;

  value = value_create (VALUE_FLOAT, ast->location);

  value->value.f = ast->value.token.value.f;

  return value;
}

struct value *
evaluate_array (struct ast *ast, struct table *table)
{
  (void)table;

  struct value *value;

  value = value_create_e (error_create ("intentional error"), ast->location);

  return value;
}

struct value *
evaluate_variable_assignment (struct ast *ast, struct table *table)
{
  struct ast *current = ast->child;

  const char *name = current->value.token.value.s;

  struct value *value;

  value = evaluate (current->next, table);
  if (value_match_error (value))
    return value;

  table_append (table, table_entry_create (name, value));

  return value;
}

struct value *
evaluate_function_definition (struct ast *ast, struct table *table)
{
  struct value *value;

  value = value_create (VALUE_FUNCTION, ast->location);

  value->value.function = function_create (ast, table);

  return value;
}

struct value *
evaluate_function_invocation (struct ast *ast, struct table *table)
{
  struct value *f = evaluate (ast->child, table);
  if (value_match_error (f))
    return f;

  if (value_match (f, VALUE_FUNCTION))
    {
      struct function *function = f->value.function;

      struct table *f_table = table_create (4, function->table);

      struct value *argument = evaluate (ast->child->next, table);
      if (value_match_error (argument))
        {
          value_destroy (f);
          table_destroy (f_table);
          return argument;
        }

      const char *name = function->ast->child->value.token.value.s;
      table_append (f_table, table_entry_create (name, argument));

      struct value *result = evaluate (function->ast->child->next, f_table);
      if (value_match_error (result))
        {
          value_destroy (argument);
          value_destroy (f);
          table_destroy (f_table);
          return result;
        }

      value_destroy (argument);
      value_destroy (f);
      table_destroy (f_table);

      return result;
    }

  struct error e = error_create ("attempted to call %s",
                                 value_type_string (f->type));

  value_destroy (f);

  return value_create_e (e, ast->location);
}

struct value *
evaluate_program (struct ast *ast, struct table *table)
{
  struct value *value = NULL;
  struct ast *current = ast->child;

  while (current != NULL)
    {
      value = evaluate (current, table);
      if (value_match_error (value))
        return value;

      current = current->next;

      if (current != NULL)
        value_destroy (value);
    }

  if (value == NULL)
    value = value_create (VALUE_NOTHING, ast->location);

  return value;
}

