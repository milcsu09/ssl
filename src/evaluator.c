#include "array.h"
#include "ast.h"
#include "evaluator.h"
#include "function.h"
#include "string.h"
#include "native.h"
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

  // struct value *copy = value_copy (value);
  // value_retain (copy);
  // return copy;
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

  value = value_create (VALUE_ARRAY, ast->location);

  size_t n;
  struct ast *current;

  n = 0;
  current = ast->child;
  while (current != NULL)
    current = current->next, ++n;

  struct value **body = calloc (n, sizeof (struct value *));

  n = 0;
  current = ast->child;
  while (current != NULL)
    {
      struct value *item = evaluate (current, table);
      if (value_match_error (item))
        {
          free (value);
          printf ("%ld\n", n);
          for (size_t i = 0; i < n; ++i)
            value_destroy (body[i]);
          free (body);
          return item;
        }

      body[n++] = item;
      current = current->next;
    }

  value->value.array = array_create (body, n);

  for (size_t i = 0; i < n; ++i)
    value_destroy (body[i]);
  free (body);

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

      struct table *f_table = table_copy (function->table);

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

      // value_retain (result);
      return result;
    }

  if (value_match (f, VALUE_NATIVE))
    {
      struct value *native_v = value_copy (f);
      struct native *native = native_v->value.native;

      struct value *argument = evaluate (ast->child->next, table);
      if (value_match_error (argument))
        {
          value_destroy (f);
          value_destroy (native_v);
          return argument;
        }

      native_c_function_t c_function = native->function;
      struct array *arguments = array_push_back (native->arguments, argument);

      array_destroy (native->arguments);
      native->arguments = arguments;

      struct value *result = c_function (native_v);

      /* In case the native function didn't return the same pointer (itself), 
       * we assume that the native got enough arguments! */
      if (result != native_v)
        value_destroy (native_v);

      value_destroy (argument);
      value_destroy (f);

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

  // value_retain (value);
  return value;
}

