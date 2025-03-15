#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

int
main (void)
{
  struct arena lexer_arena = {0};
  struct lexer lexer = lexer_create ("a + (  ", "__tmp__", &lexer_arena);

  struct arena parser_arena = {0};
  struct parser parser = parser_create (&lexer, &parser_arena);

  struct ast *ast = parser_parse (&parser);

  ast_debug_print (ast, 0);

  arena_destroy (&parser_arena);
  arena_destroy (&lexer_arena);

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
  struct arena lexer_arena = {0};
  struct lexer lexer = lexer_create ("almaspite + kortebanan", "<>", &lexer_arena);
  struct token token;

  while (1)
    {
      token = lexer_next (&lexer);
      if (token.type == TOKEN_NOTHING || token.type == TOKEN_ERROR)
        break;

      // printf (
      //   "%s:%ld:%ld: ",
      //   token.location.context,
      //   token.location.line,
      //   token.location.column
      // );

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

      // token_destroy (token);
      printf ("\n");
    }

  if (token.type == TOKEN_ERROR)
    {
      printf (
        "%s:%ld:%ld: fatal-error: %s\n",
        token.location.context,
        token.location.line,
        token.location.column,
        token.value.e.message
      );
    }

  arena_destroy (&lexer_arena);
  */

  return 0;
}

