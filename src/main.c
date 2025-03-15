#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "parser.h"

int
main (void)
{
  struct arena lexer_arena = {0};
  struct lexer lexer = lexer_create ("a + ? * c;", "__tmp__", &lexer_arena);

  struct arena parser_arena = {0};
  struct parser parser = parser_create (&lexer, &parser_arena);

  struct ast *ast = parser_parse (&parser);

  ast_debug_print (ast, 0);

  arena_destroy (&lexer_arena);
  arena_destroy (&parser_arena);

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

