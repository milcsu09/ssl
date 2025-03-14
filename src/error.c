#include "error.h"
#include <stdarg.h>
#include <stdio.h>

void
location_advance (struct location *location, char c)
{
  location->column++;
  if (c == '\n')
    location->column = 1, location->line++;
}

struct error
error_create (const char *fmt, ...)
{
  struct error error;
  va_list va;

  va_start (va, fmt);
  vsnprintf (error.message, sizeof (error.message), fmt, va);
  va_end (va);

  return error;
}

