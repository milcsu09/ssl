#include "tree.h"

#include <stdio.h>


static const char *const TREE_KIND_STRING[] = {
  "Program",
  "Assignment",
  "Lambda",
  "Match",
  "Match Arm",
  "Branch",
  "Call",
  "Call Binary",
  "Unit",
  "List",
  "Integer",
  "Float",
  "String",
  "Name",
  "Pattern Wildcard",
  "Pattern Name",
  "Pattern Literal",
  "Pattern List Empty",
  "Pattern List Cons",
};


const char *
tree_kind_string (enum tree_kind kind)
{
  return TREE_KIND_STRING[kind];
}


struct tree *
tree_create (struct state *state, struct location location, enum tree_kind kind)
{
  struct tree *tree;

  tree = state_aa_malloc (state, sizeof (struct tree));

  state_unhealthy_return2 (state, NULL);

  tree->location = location;
  tree->kind = kind;

  tree->token = NULL;

  tree->child = NULL;
  tree->next = NULL;

  return tree;
}


void
tree_attach (struct tree *tree, struct tree *node)
{
  struct tree *current = tree;

  while (current->next)
    current = current->next;

  current->next = node;
}


void
tree_append (struct tree *tree, struct tree *node)
{
  if (!tree->child)
    tree->child = node;
  else
    tree_attach (tree->child, node);
}


void
tree_debug_print_base (struct tree *tree, usize previous_line, usize indent)
{
  if (!tree)
    return;

  usize line = tree->location.line;

  if (previous_line != line)
    printf ("%*zu ", (s32)indent + 4, line);
  else
    printf ("%*s ", (s32)indent + 4, "");

  printf ("\033[96m%-11s\033[0m ", tree_kind_string (tree->kind));

  struct token *token = tree->token;

  if (token)
    switch (token->kind)
      {
      case TOKEN_INTEGER:
        printf ("(\033[92m%ld\033[0m)", token->data.i);
        break;
      case TOKEN_FLOAT:
        printf ("(\033[92m%f\033[0m)", token->data.f);
        break;
      case TOKEN_STRING:
        printf ("(\033[92m\"%s\"\033[0m)", token->data.s);
        break;
      case TOKEN_NAME:
        printf ("(\033[92m%s\033[0m)", token->data.s);
        break;
      default:
        printf ("(\033[92m%s\033[0m)", token_kind_string (token->kind));
        break;
      }

  printf ("\n");

  tree_debug_print_base (tree->child, line, indent + 4);
  tree_debug_print_base (tree->next, line, indent);
}


void
tree_debug_print (struct tree *tree)
{
  tree_debug_print_base (tree, 0, 0);
}

