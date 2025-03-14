#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

int
main (void)
{
  struct parser parser = parser_create ("[[1,2,3],[4,5,6],[7,8,9]];", "__tmp__");
  struct ast *ast = parser_parse (&parser);

  if (ast_match_error (ast))
    {
      location_debug_print (ast->location);
      printf (": fatal-error: %s\n", ast->value.error.message);
      ast_destroy (ast);
      parser_cleanup (&parser);
      exit (1);
    }

  ast_debug_print (ast, 0);
  ast_destroy (ast);

  /*
  struct lexer lexer = lexer_create ("1+2*34", "<>");
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

      token_destroy (token);
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
  */

  return 0;
}

