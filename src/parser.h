#ifndef PARSER_H
#define PARSER_H

#include "arena.h"
#include "ast.h"
#include "lexer.h"

struct parser
{
  struct lexer *lexer;
  struct token current;
  struct location location;
  struct arena *arena;
};

struct parser parser_create (struct lexer *lexer, struct arena *);
// void parser_cleanup (struct parser *);
struct ast *parser_parse (struct parser *);

#endif // PARSER_H

