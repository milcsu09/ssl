#ifndef PARSER_H
#define PARSER_H


#include "tree.h"
#include "lexer.h"


struct parser
{
  struct lexer *lexer;

  struct token *current;
};


struct parser *
parser_create (struct state *state, const char *const context, const char *source);

struct tree *
parser_parse (struct state *state, struct parser *parser);


#endif // PARSER_H

