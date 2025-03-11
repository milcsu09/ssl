#ifndef ERROR_H
#define ERROR_H


#include <stddef.h>
#include <stdio.h>


struct location
{
  const char *context;

  size_t line;
};


struct error
{
  char message[256];

  struct location location;
};


struct error error_create (struct location, const char *, ...);


#endif // ERROR_H

