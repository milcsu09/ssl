#ifndef ERROR_H
#define ERROR_H

#include <stddef.h>

struct location
{
  const char *context;
  size_t line;
  size_t column;
};

enum error_type
{
  ERROR_NOTHING,
  ERROR_SYNTAX,
  ERROR_RUNTIME,
};

struct error
{
  char message[256];
  enum error_type type;
  struct location location;
};

void location_advance (struct location *, char);
const char *error_type_string (enum error_type);

#endif // ERROR_H

