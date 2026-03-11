#ifndef TOKEN_H
#define TOKEN_H


#include "state.h"


enum token_kind
{
  TOKEN_EOF,

  TOKEN_EQUALS,
  TOKEN_SEMICOLON,

  TOKEN_COMMA,
  TOKEN_DOT,

  TOKEN_MATCH,
  TOKEN_PIPE,

  TOKEN_LPAREN,
  TOKEN_RPAREN,
  TOKEN_LBRACKET,
  TOKEN_RBRACKET,
  TOKEN_LBRACE,
  TOKEN_RBRACE,

  TOKEN_INTEGER,
  TOKEN_FLOAT,

  TOKEN_STRING,
  TOKEN_NAME,
};


union token_data
{
  s64 i;

  f64 f;

  char *s;
};


// NOTE: No out-of-bounds check!
const char *
token_kind_string (enum token_kind kind);


struct token
{
  struct location location;

  union token_data data;

  enum token_kind kind;
};


struct token *
token_create (struct state *state, struct location location, enum token_kind kind);

struct token *
token_box_i (struct state *state, struct location location, enum token_kind kind, s64 i);

struct token *
token_box_f (struct state *state, struct location location, enum token_kind kind, f64 f);

struct token *
token_box_s (struct state *state, struct location location, enum token_kind kind, char *s);

s64
token_unbox_i (struct token *token);

f64
token_unbox_f (struct token *token);

char *
token_unbox_s (struct token *token);

bool
token_match (struct token *token, enum token_kind kind);

bool
token_match_integer (struct token *token, s64 i);

bool
token_match_float (struct token *token, f64 f);

bool
token_match_string (struct token *token, const char *s);

bool
token_match_name (struct token *token, const char *s);


#endif // TOKEN_H

