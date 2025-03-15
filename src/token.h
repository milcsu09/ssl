#ifndef TOKEN_H
#define TOKEN_H

#include "arena.h"
#include "error.h"

union token_entry
{
  struct error e;
  long i;
  double f;
  char *s;
};

enum token_type
{
  TOKEN_NOTHING,
  TOKEN_ERROR,
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
struct token token_create_e (struct error, struct location);
struct token token_create_i (long, enum token_type, struct location);
struct token token_create_f (double, enum token_type, struct location);
struct token token_create_s (char *, enum token_type, struct location);
struct token token_copy (struct token, struct arena *);
int token_match (struct token, enum token_type);
int token_match_error (struct token);
void token_debug_print (struct token);

#endif // TOKEN_H

