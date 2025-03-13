#include <stdio.h>
#include "lexer.h"

int
main (void)
{
  struct lexer lexer = lexer_create (" ( x = 51 ) ", "<>");
  struct token token;

  while ((token = lexer_next (&lexer)).type != TOKEN_NOTHING)
    {
      printf (
        "%s:%ld:%ld: ",
        token.location.context,
        token.location.line,
        token.location.column
      );

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

  if (lexer.error.type != ERROR_NOTHING)
    {
      printf (
        "%s:%ld:%ld: %s [%s]\n",
        lexer.error.location.context,
        lexer.error.location.line,
        lexer.error.location.column,
        lexer.error.message,
        error_type_string (lexer.error.type)
      );
    }

  return 0;
}

