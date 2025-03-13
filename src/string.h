#ifndef STRING_H
#define STRING_H

#include <stddef.h>

char *string_copy (const char *);
char *string_copy_n (const char *, size_t);
char *string_copy_until (const char *, const char *);
void string_escape (char *);

#endif // STRING_H

