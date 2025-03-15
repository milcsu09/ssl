#include <stdio.h>
#include <stdlib.h>

#include "evaluator.h"
#include "table.h"
#include "arena.h"
#include "ast.h"
#include "parser.h"
#include "value.h"

int
main (void)
{
  // printf ("%ld\n", sizeof (struct table));

  struct arena lexer_arena = {0};
  struct lexer lexer = lexer_create ("f = a -> b -> a; f1 = f 1; f 1 512;", "__tmp__", &lexer_arena);

  struct arena parser_arena = {0};
  struct parser parser = parser_create (&lexer, &parser_arena);

  struct ast *ast = parser_parse (&parser);

  if (ast_match_error (ast))
    {
      location_debug_print (ast->location);
      printf (": fatal-error: %s [SYNTAX]\n", ast->value.error.message);

      arena_destroy (&lexer_arena);
      arena_destroy (&parser_arena);

      exit (1);
    }
  // else
  //   ast_debug_print (ast, 0);

  struct table *gst = table_create (4, NULL);
  struct value *value = evaluate (ast, gst);

  if (value_match_error (value))
    {
      location_debug_print (value->location);
      printf (": fatal-error: %s [RUNTIME]\n", value->value.error.message);

      arena_destroy (&lexer_arena);
      arena_destroy (&parser_arena);

      value_destroy (value);
      table_destroy (gst);

      exit (1);
    }

  printf ("\nevaluation returned with %s type\n",
          value_type_string (value->type));
  printf ("\n");
  value_debug_print (value);
  printf ("\n\n");

  arena_destroy (&lexer_arena);
  arena_destroy (&parser_arena);

  for (size_t i = 0; i < gst->size; ++i)
    {
      printf ("%s=", gst->storage[i]->key);
      value_debug_print (gst->storage[i]->value);
      printf ("\n");
    }

  value_destroy (value);
  table_destroy (gst);

  /*struct parser parser = parser_create ("a + b +", "__tmp__");
  struct ast *ast = parser_parse (&parser);

  if (ast_match_error (ast))
    {
      location_debug_print (ast->location);
      printf (": fatal-error: %s\n", ast->value.error.message);
      ast_destroy (ast);
      // parser_cleanup (&parser);
      exit (1);
    }

  ast_debug_print (ast, 0);
  ast_destroy (ast);
  */

  /*
  struct lexer lexer = lexer_create ("a + b", "<>", NULL);
  struct token token;

  while (1)
    {
      token = lexer_next (&lexer);
      if (token.type == TOKEN_NOTHING || token.type == TOKEN_ERROR)
        break;

      printf ("'%s'", token_type_string (token.type));
      switch (token.type)
        {
        case TOKEN_IDENTIFIER:
          printf (": `%s`", token.value.s);
          break;
        case TOKEN_STRING:
          printf (": \"%s\"", token.value.s);
          break;
        case TOKEN_INTEGER:
          printf (": %ld", token.value.i);
          break;
        case TOKEN_FLOAT:
          printf (": %lf", token.value.f);
          break;
        default:
          break;
        }

      token_destroy (token);
      printf ("\n");
    }
  */

  return 0;
}

