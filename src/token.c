#include "string.h"
#include "token.h"
#include <stdlib.h>

static const char *const TOKEN_TYPE_STRING[] = {
  "nothing",
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
  ",",
};

const char *
token_type_string (enum token_type type)
{
  return TOKEN_TYPE_STRING[type];
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

  token.value.i = i;

  return token;
}

struct token
token_create_f (double f, enum token_type type, struct location location)
{
  struct token token;

  token = token_create (type, location);

  token.value.f = f;

  return token;
}

struct token
token_create_s (char *s, enum token_type type, struct location location)
{
  struct token token;

  token = token_create (type, location);

  token.value.s = s;

  return token;
}

struct token
token_copy (struct token token)
{
  struct token copy;

  switch (token.type)
    {
    case TOKEN_IDENTIFIER:
      copy.value.s = string_copy (token.value.s);
      break;
    case TOKEN_STRING:
      copy.value.s = string_copy (token.value.s);
      break;
    case TOKEN_INTEGER:
      copy.value.i = token.value.i;
      break;
    case TOKEN_FLOAT:
      copy.value.i = token.value.i;
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
      free (token.value.s);
      break;
    case TOKEN_STRING:
      free (token.value.s);
      break;
    default:
      break;
    }
}

