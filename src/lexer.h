#ifndef LEXER_H
#define LEXER_H


#include "token.h"


struct lexer
{
  struct location location;

  const char *current;
};


struct lexer *
lexer_create (struct state *state, const char *const context, const char *current);

struct token *
lexer_next (struct state *state, struct lexer *lexer);

struct token *
lexer_peek (struct state *state, struct lexer *lexer);


#endif // LEXER_H

