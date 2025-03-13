#ifndef LEXER_H
#define LEXER_H

#include "error.h"
#include "token.h"

struct lexer
{
  char *current;
  struct location location;
  struct error error;
};

struct lexer lexer_create (char *const source, const char *const context);
struct token lexer_next (struct lexer *lexer);
struct token lexer_peek (struct lexer *lexer);

#endif // LEXER_H

