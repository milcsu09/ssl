#ifndef ERROR_H
#define ERROR_H


#include "common.h"


struct location
{
  const char *context;
  usize line;
  usize column;
};


extern const struct location location_none;


struct error
{
  char message[256];
};


#endif // ERROR_H

