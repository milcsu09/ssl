#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexer.h"

struct parser
{
  struct lexer lexer;
  struct token current;
  struct location location;
};

struct parser parser_create (char *const source, const char *const context);
void parser_destroy (struct parser *parser);
struct ast *parser_parse (struct parser *parser);

#endif // PARSER_H

