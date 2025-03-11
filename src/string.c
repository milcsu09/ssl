#include "string.h"
#include <stdlib.h>
#include <string.h>


char *
string_copy (const char *s)
{
  if (s == NULL)
    return NULL;

  size_t length = strlen (s);

  char *result = calloc (length + 1, sizeof (char));
  if (result == NULL)
    abort ();

  memcpy (result, s, length);
  return result;
}


char *
string_copy_n (const char *s, size_t n)
{
  if (s == NULL)
    return NULL;

  size_t length;
  length = strlen (s);
  length = (n < length) ? n : length;

  char *result = calloc (length + 1, sizeof (char));
  if (result == NULL)
    abort ();

  memcpy (result, s, length);
  result[length] = '\0';
  return result;
}

