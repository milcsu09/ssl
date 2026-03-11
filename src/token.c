#include "token.h"

#include <string.h>


static const char *const TOKEN_KIND_STRING[] = {
  "EOF",

  "'='",
  "';'",

  "','",
  "'.'",

  "'match'",
  "'|'",

  "'('",
  "')'",
  "'['",
  "']'",
  "'{'",
  "'}'",

  "integer",
  "float",

  "string",
  "name",
};


const char *
token_kind_string (enum token_kind kind)
{
  return TOKEN_KIND_STRING[kind];
}


struct token *
token_create (struct state *state, struct location location, enum token_kind kind)
{
  struct token *token;

  token = state_aa_malloc (state, sizeof (struct token));

  state_unhealthy_return2 (state, NULL);

  token->location = location;

  token->kind = kind;

  return token;
}


struct token *
token_box_i (struct state *state, struct location location, enum token_kind kind, s64 i)
{
  struct token *token;

  token = token_create (state, location, kind);

  state_unhealthy_return2 (state, NULL);

  token->data.i = i;

  return token;
}


struct token *
token_box_f (struct state *state, struct location location, enum token_kind kind, f64 f)
{
  struct token *token;

  token = token_create (state, location, kind);

  state_unhealthy_return2 (state, NULL);

  token->data.f = f;

  return token;
}


struct token *
token_box_s (struct state *state, struct location location, enum token_kind kind, char *s)
{
  struct token *token;

  token = token_create (state, location, kind);

  state_unhealthy_return2 (state, NULL);

  token->data.s = s;

  return token;
}


s64
token_unbox_i (struct token *token)
{
  return token->data.i;
}


f64
token_unbox_f (struct token *token)
{
  return token->data.f;
}


char *
token_unbox_s (struct token *token)
{
  return token->data.s;
}


bool
token_match (struct token *token, enum token_kind kind)
{
  return token->kind == kind;
}


bool
token_match_integer (struct token *token, s64 i)
{
  if (!token_match (token, TOKEN_INTEGER))
    return false;

  return token->data.i == i;
}


bool
token_match_float (struct token *token, f64 f)
{
  if (!token_match (token, TOKEN_FLOAT))
    return false;

  return token->data.f == f;
}


bool
token_match_string (struct token *token, const char *s)
{
  if (!token_match (token, TOKEN_STRING))
    return false;

  return strcmp (token->data.s, s) == 0;
}


bool
token_match_name (struct token *token, const char *s)
{
  if (!token_match (token, TOKEN_NAME))
    return false;

  return strcmp (token->data.s, s) == 0;
}

