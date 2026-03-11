#include "string.h"

#include <string.h>


char *
c_string_copy (struct state *state, const char *s, bool use_aa)
{
  if (s == NULL)
    return NULL;

  usize length = strlen (s);

  char *copy;

  if (use_aa)
    copy = state_aa_malloc (state, length + 1);
  else
    copy = state_malloc (state, length + 1);

  state_unhealthy_return2 (state, NULL);

  memcpy (copy, s, length);

  copy[length] = '\0';

  return copy;
}


char *
c_string_copy_n (struct state *state, const char *s, usize length, bool use_aa)
{
  if (s == NULL)
    return NULL;

  char *copy;

  if (use_aa)
    copy = state_aa_malloc (state, length + 1);
  else
    copy = state_malloc (state, length + 1);

  state_unhealthy_return2 (state, NULL);

  memcpy (copy, s, length);

  copy[length] = '\0';

  return copy;
}


char *
c_string_copy_until (struct state *state, const char *s, const char *e, bool use_aa)
{
  return c_string_copy_n (state, s, e - s, use_aa);
}


void
c_string_escape (char *s)
{
  if (s == NULL)
    return;

  char *i = s, *j = s;

  while (*i != '\0')
    {
      if (*i == '\\')
        {
          i++;
          switch (*i)
            {
            case 'a':
              *j = '\a';
              break;
            case 'b':
              *j = '\b';
              break;
            case 'f':
              *j = '\f';
              break;
            case 'n':
              *j = '\n';
              break;
            case 'r':
              *j = '\r';
              break;
            case 't':
              *j = '\t';
              break;
            case 'v':
              *j = '\v';
              break;
            case '\\':
              *j = '\\';
              break;
            case '\'':
              *j = '\'';
              break;
            case '\"':
              *j = '\"';
              break;
            default:
              *j = '\\';
              *++j = *i;
              break;
            }

          i++;
        }
      else
        *j = *i++;

      j++;
    }

  *j = '\0';
}

