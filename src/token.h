#ifndef TOKEN_H
#define TOKEN_H


#include "error.h"
#include <stddef.h>


union token_entry
{
  long i;

  double f;

  char *s;
};


enum token_type
{
  TOKEN_EOF,

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

  TOKEN_PIPE,

  TOKEN_QUESTION,
};


const char *token_type_string (enum token_type);


struct token
{
  union token_entry entry;

  enum token_type type;

  struct location location;
};


struct token token_create (enum token_type, struct location);

struct token token_create_i (long, enum token_type, struct location);

struct token token_create_f (double, enum token_type, struct location);

struct token token_create_s (char *, enum token_type, struct location);

struct token token_copy (struct token);

void token_destroy (struct token);


#endif // TOKEN_H

