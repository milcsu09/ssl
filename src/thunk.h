#ifndef THUNK_H
#define THUNK_H

struct thunk
{
  struct ast *ast;
  struct table *table;

  /* TODO: cache result
   * struct value *value;
   * int cached; */
};

struct thunk *thunk_create (struct ast *, struct table *);
struct thunk *thunk_copy (struct thunk *);

void thunk_destroy (struct thunk *);

#endif // THUNK_H

