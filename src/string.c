#include "string.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

char *
string_copy (const char *s)
{
  if (s == NULL)
    return NULL;

  size_t length = strlen (s);

  char *copy = calloc (length + 1, sizeof (char));

  if (copy)
    memcpy (copy, s, length);

  return copy;
}

char *
string_copy_n (const char *s, size_t length)
{
  if (s == NULL)
    return NULL;

  char *copy = calloc (length + 1, sizeof (char));

  if (copy)
    memcpy (copy, s, length);

  return copy;
}

char *
string_copy_until (const char *s, const char *e)
{
  return string_copy_n (s, e - s);
}

void
string_escape (char *s)
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

