#ifndef TOKEN_H
#define TOKEN_H

#include "error.h"

#define TOKEN_EMPTY token_create (0, 0)

union token_entry
{
  long i;
  double f;
  char *s;
};

enum token_type
{
  TOKEN_NOTHING,
  TOKEN_SEMICOLON,
  TOKEN_IDENTIFIER,
  TOKEN_STRING,
  TOKEN_INTEGER,
  TOKEN_FLOAT,
  TOKEN_LPAREN,
  TOKEN_RPAREN,
  TOKEN_LBRACKET,
  TOKEN_RBRACKET,
  TOKEN_LBRACE,
  TOKEN_RBRACE,
  TOKEN_EQUAL,
  TOKEN_ARROW,
  TOKEN_COMMA,
  // TOKEN_PIPE,
  // TOKEN_QUESTION,
};

struct token
{
  union token_entry value;
  enum token_type type;
  struct location location;
};

const char *token_type_string (enum token_type);
struct token token_create (enum token_type, struct location);
struct token token_create_i (long, enum token_type, struct location);
struct token token_create_f (double, enum token_type, struct location);
struct token token_create_s (char *, enum token_type, struct location);
struct token token_copy (struct token);
void token_destroy (struct token);

#endif // TOKEN_H

