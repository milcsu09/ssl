#ifndef STANDARD_H
#define STANDARD_H

/*
 * TODO: Standard functions should check for argument types.
*/

/* Function for testing purposes. */
struct value *standard_f_f (struct value *);

struct value *standard_f_print (struct value *);
struct value *standard_f_printl (struct value *);

struct value *standard_f_add (struct value *);
struct value *standard_f_sub (struct value *);
struct value *standard_f_mul (struct value *);
struct value *standard_f_div (struct value *);
struct value *standard_f_mod (struct value *);

struct value *standard_f_eq (struct value *);
struct value *standard_f_neq (struct value *);

struct value *standard_f_head (struct value *);
struct value *standard_f_tail (struct value *);
struct value *standard_f_push_front (struct value *);

struct value *standard_f_range (struct value *);

struct value *standard_f_map (struct value *);
struct value *standard_f_map_ (struct value *);

struct value *standard_f_error (struct value *);

#endif // STANDARD_H

