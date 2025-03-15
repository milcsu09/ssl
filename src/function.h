#ifndef FUNCTION_H
#define FUNCTION_H

struct function
{
  struct ast *ast;
  struct table *table;
};

struct function *function_create (struct ast *, struct table *);
struct function *function_copy (struct function *);

void function_destroy (struct function *);

#endif // FUNCTION_H

