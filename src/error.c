#include "error.h"

void
location_advance (struct location *location, char c)
{
  location->column++;
  if (c == '\n')
    location->column = 1, location->line++;
}

static const char *const ERROR_TYPE_STRING[] = {
  "NOTHING",
  "SYNTAX",
  "RUNTIME",
};

const char *
error_type_string (enum error_type type)
{
  return ERROR_TYPE_STRING[type];
}

