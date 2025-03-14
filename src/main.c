#include <stdio.h>
#include "lexer.h"

int
main (void)
{

  struct lexer lexer = lexer_create ("(x = \"ab)", "<>");
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

  return 0;
}

