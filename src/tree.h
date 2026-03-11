#ifndef TREE_H
#define TREE_H


#include "token.h"


enum tree_kind
{
  TREE_PROGRAM,

  TREE_ASSIGNMENT,

  TREE_LAMBDA,

  TREE_MATCH,
  TREE_MATCH_ARM,

  TREE_BRANCH,

  TREE_CALL,
  TREE_CALL_BINARY,

  TREE_UNIT,
  TREE_LIST,

  TREE_INTEGER,
  TREE_FLOAT,

  TREE_STRING,
  TREE_NAME,

  // Patterns
  TREE_PATTERN_WILDCARD,
  TREE_PATTERN_NAME,
  TREE_PATTERN_LITERAL,
  TREE_PATTERN_LIST_EMPTY,
  TREE_PATTERN_LIST_CONS,
};


// NOTE: No out-of-bounds check!
const char *
tree_kind_string (enum tree_kind kind);


struct tree
{
  struct location location;

  struct token *token;

  struct tree *child;

  struct tree *next;

  enum tree_kind kind;
};


struct tree *
tree_create (struct state *state, struct location location, enum tree_kind kind);

void
tree_attach (struct tree *tree, struct tree *node);

void
tree_append (struct tree *tree, struct tree *node);

void
tree_debug_print (struct tree *tree);


#endif // TREE_H

