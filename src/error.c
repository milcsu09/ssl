#include "error.h"
#include <stdarg.h>


struct error
error_create (struct location location, const char *format, ...)
{
  struct error error;

  va_list va;
  va_start (va, format);

  vsnprintf (error.message, sizeof error.message, format, va);

  va_end (va);

  error.location = location;

  return error;
}


