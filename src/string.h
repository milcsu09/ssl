#ifndef STRING_H
#define STRING_H


#include "state.h"


char *
c_string_copy (struct state *state, const char *s, bool use_aa);

char *
c_string_copy_n (struct state *state, const char *s, usize length, bool use_aa);

char *
c_string_copy_until (struct state *state, const char *s, const char *e, bool use_aa);

void
c_string_escape (char *s);


#endif // STRING_H


