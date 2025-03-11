#include "token.h"
#include "string.h"
#include <stdlib.h>


static const char *const TOKEN_TYPES[] = {
  "EOF",
  ";",
  "identifier",
  "string",
  "integer",
  "float",
  "(",
  ")",
  "[",
  "]",
  "{",
  "}",
  "=",
  "->",
  "|",
  "?",
};


const char *
token_type_string (enum token_type type)
{
  return TOKEN_TYPES[type];
}


struct token
token_create (enum token_type type, struct location location)
{
  struct token token;

  token.type = type;
  token.location = location;

  return token;
}


struct token
token_create_i (long i, enum token_type type, struct location location)
{
  struct token token;

  token = token_create (type, location);
  token.entry.i = i;

  return token;
}


struct token
token_create_f (double f, enum token_type type, struct location location)
{
  struct token token;

  token = token_create (type, location);
  token.entry.f = f;

  return token;
}


struct token
token_create_s (char *s, enum token_type type, struct location location)
{
  struct token token;

  token = token_create (type, location);
  token.entry.s = s;

  return token;
}


struct token
token_copy (struct token token)
{
  struct token copy;

  switch (token.type)
    {
    case TOKEN_IDENTIFIER:

    case TOKEN_STRING:
      copy.entry.s = string_copy (token.entry.s);
      break;

    case TOKEN_INTEGER:
      copy.entry.i = token.entry.i;
      break;

    case TOKEN_FLOAT:
      copy.entry.f = token.entry.f;
      break;

    default:
      break;
    }

  copy.type = token.type;
  copy.location = token.location;

  return copy;
}


void
token_destroy (struct token token)
{
  switch (token.type)
    {
    case TOKEN_IDENTIFIER:

    case TOKEN_STRING:
      free (token.entry.s);
      break;

    default:
      break;
    }
}

