#include "lexer.h"
#include "string.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>


struct lexer *
lexer_create (struct state *state, const char *const context, const char *current)
{
  struct lexer *lexer;

  lexer = state_aa_malloc (state, sizeof (struct lexer));

  state_unhealthy_return2 (state, NULL);

  lexer->location.context = context;
  lexer->location.line = 1;
  lexer->location.column = 1;

  lexer->current = current;

  return lexer;
}


void
lexer_advance (struct lexer *lexer)
{
  if (*lexer->current == '\0')
    return;

  char c = *lexer->current++;

  if (*lexer->current == '\0')
    return;

  if (c == '\n')
    {
      lexer->location.line++;
      lexer->location.column = 1;
    }
  else
    lexer->location.column++;
}


void
lexer_advance_n (struct lexer *lexer, usize n)
{
  while (n--)
    lexer_advance (lexer);
}


struct token *
lexer_advance_token (struct state *state, struct lexer *lexer, enum token_kind kind)
{
  struct token *token;

  token = token_create (state, lexer->location, kind);

  state_unhealthy_return2 (state, NULL);

  lexer_advance (lexer);

  return token;
}


struct token *
lexer_advance_n_token (struct state *state, struct lexer *lexer, usize n, enum token_kind kind)
{
  struct token *token;

  token = token_create (state, lexer->location, kind);

  state_unhealthy_return2 (state, NULL);

  lexer_advance_n (lexer, n);

  return token;
}


bool
lexer_is_digit (char c, int base)
{
  switch (base)
    {
    case 2:
      return c == '0' || c == '1';
    case 8:
      return c >= '0' && c <= '7';
    case 16:
      return isxdigit ((unsigned char)c);
    }

  return false;
}


struct token *
lexer_next (struct state *state, struct lexer *lexer)
{
  while (isspace ((unsigned char)*lexer->current))
    lexer_advance (lexer);

  while (*lexer->current == '#')
    {
      while (*lexer->current && *lexer->current != '\n')
        lexer_advance (lexer);

      while (isspace ((unsigned char)*lexer->current))
        lexer_advance (lexer);
    }

  char c = *lexer->current;

  if (c == '\0')
    return token_create (state, lexer->location, TOKEN_EOF);

  struct location location = lexer->location;

  const char *start = lexer->current;

  // Number
  if (isdigit ((unsigned char)c) || (c == '.' && isdigit ((unsigned char)lexer->current[1])))
    {
      bool is_float = false;

      while (isdigit (*lexer->current))
        lexer_advance (lexer);

      if (*lexer->current == '.')
        {
          is_float = true;
          lexer_advance (lexer);

          while (isdigit (*lexer->current))
            lexer_advance (lexer);
        }

      if (*lexer->current == 'e' || *lexer->current == 'E')
        {
          is_float = true;
          lexer_advance (lexer);

          if (*lexer->current == '+' || *lexer->current == '-')
            lexer_advance (lexer);

          while (isdigit (*lexer->current))
            lexer_advance (lexer);
        }

      if (is_float)
        {
          f64 x = strtod (start, NULL);
          return token_box_f (state, location, TOKEN_FLOAT, x);
        }
      else
        {
          s64 x = strtol (start, NULL, 10);
          return token_box_i (state, location, TOKEN_INTEGER, x);
        }
    }

  // String
  if (c == '"')
    {
      lexer_advance (lexer);

      while (*lexer->current && *lexer->current != '"' && *lexer->current != '\n')
        {
          if (*lexer->current == '\\')
            lexer_advance (lexer);
          lexer_advance (lexer);
        }

      if (*lexer->current != '"')
        {
          state_set_location (state, location);

          state_set_error (state, "unterminated string-literal");

          return NULL;
        }

      lexer_advance (lexer);

      // NOTE: +1 / -1 for skipping leading / trailing '"'.
      char *s = c_string_copy_until (state, start + 1, lexer->current - 1, true);

      state_unhealthy_return2 (state, NULL);

      c_string_escape (s);

      return token_box_s (state, location, TOKEN_STRING, s);
    }

  // Name, Alphanumeric
  if (isalpha (*lexer->current) || *lexer->current == '_')
    {
      while (isalnum ((unsigned char)*lexer->current) || *lexer->current == '_' || *lexer->current == '\'')
        lexer_advance (lexer);

      size_t length = lexer->current - start;

      if (length == 5 && strncmp (start, "match", 5) == 0)
        return token_create (state, location, TOKEN_MATCH);

      char *s = c_string_copy_until (state, start, lexer->current, true);

      state_unhealthy_return2 (state, NULL);

      return token_box_s (state, location, TOKEN_NAME, s);
    }

  // Name, Punctuation
  if (ispunct ((unsigned char)c))
    {
      // Hard reserved punctuation
      switch (c)
        {
        case ';':
          return lexer_advance_token (state, lexer, TOKEN_SEMICOLON);
        case ',':
          return lexer_advance_token (state, lexer, TOKEN_COMMA);
        case '(':
          return lexer_advance_token (state, lexer, TOKEN_LPAREN);
        case ')':
          return lexer_advance_token (state, lexer, TOKEN_RPAREN);
        case '[':
          return lexer_advance_token (state, lexer, TOKEN_LBRACKET);
        case ']':
          return lexer_advance_token (state, lexer, TOKEN_RBRACKET);
        case '{':
          return lexer_advance_token (state, lexer, TOKEN_LBRACE);
        case '}':
          return lexer_advance_token (state, lexer, TOKEN_RBRACE);
        default:
          break;
        }

      usize length = 0;

      // Gather characters until separator or non-punctuation.
      for (bool done = false; !done;)
        switch (start[length])
          {
          // Separators
          case '#':
          case '"':

          case ';':
          case ',':
          case '(':
          case ')':
          case '[':
          case ']':
          case '{':
          case '}':
            // NOTE: Don't increase length, these are handled elsewhere.
            done = true;
            break;
          default:
            if (ispunct ((unsigned char)start[length]))
              {
                length++;
                continue;
              }

            done = true;
            break;
          }

      // Soft reserved punctuation
      switch (length)
        {
        case 1:
          switch (start[0])
            {
            case '=':
              return lexer_advance_token (state, lexer, TOKEN_EQUALS);
            case '.':
              return lexer_advance_token (state, lexer, TOKEN_DOT);
            case '|':
              return lexer_advance_token (state, lexer, TOKEN_PIPE);
            }
          break;
        default:
          break;
        }

      char *s = c_string_copy_n (state, start, length, true);

      state_unhealthy_return2 (state, NULL);

      struct location location = lexer->location;

      lexer_advance_n (lexer, length);

      return token_box_s (state, location, TOKEN_NAME, s);
    }

  state_set_location (state, lexer->location);

  state_set_error (state, "unexpected character");

  return NULL;
}


struct token *
lexer_peek (struct state *state, struct lexer *lexer)
{
  struct lexer copy = *lexer;

  return lexer_next (state, &copy);
}

