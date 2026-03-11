#include "parser.h"

#include <stdbool.h>
#include <stdint.h>

#include <stdio.h>


enum
{
  ASSOCIATIVITY_L,
  ASSOCIATIVITY_R,
};


struct operator
{
  const char *s;
  int p;
  int a;
};


static struct operator OPERATOR_TABLE[] = {
  { ":",   2, ASSOCIATIVITY_R },

  { "==", 30, ASSOCIATIVITY_L },
  { "/=", 30, ASSOCIATIVITY_L },

  { "<",  40, ASSOCIATIVITY_L },
  { ">",  40, ASSOCIATIVITY_L },
  { "<=", 40, ASSOCIATIVITY_L },
  { ">=", 40, ASSOCIATIVITY_L },

  { "+",  50, ASSOCIATIVITY_L },
  { "-",  50, ASSOCIATIVITY_L },

  { "*",  60, ASSOCIATIVITY_L },
  { "/",  60, ASSOCIATIVITY_L },
  { "%",  60, ASSOCIATIVITY_L },
};


enum
{
  OPERATOR_TABLE_SIZE = sizeof (OPERATOR_TABLE) / sizeof (struct operator),
};


struct operator
parser_fetch_operator (struct token *token)
{
  if (!token_match (token, TOKEN_NAME))
    goto invalid;

  for (int i = 0; i < OPERATOR_TABLE_SIZE; ++i)
    if (token_match_name (token, OPERATOR_TABLE[i].s))
      return OPERATOR_TABLE[i];

invalid:
  return (struct operator){ 0 };
}


struct parser *
parser_create (struct state *state, const char *const context, const char *source)
{
  struct lexer *lexer;

  lexer = lexer_create (state, context, source);

  state_unhealthy_return2 (state, NULL);

  struct parser *parser;

  parser = state_aa_malloc (state, sizeof (struct parser));

  state_unhealthy_return2 (state, NULL);

  parser->lexer = lexer;

  return parser;
}


void
parser_advance (struct state *state, struct parser *parser)
{
  parser->current = lexer_next (state, parser->lexer);
}


bool
parser_match_kind (struct parser *parser, enum token_kind kind)
{
  return token_match (parser->current, kind);
}


void
parser_expect (struct state *state, struct parser *parser, const char *a, const char *b)
{
  state_set_location (state, parser->current->location);

  state_set_error (state, "unexpected %s, expected %s", b, a);
}


void
parser_expect_kind (struct state *state, struct parser *parser, enum token_kind kind)
{
  if (parser_match_kind (parser, kind))
    return;

  const char *a = token_kind_string (kind);
  const char *b = token_kind_string (parser->current->kind);

  parser_expect (state, parser, a, b);
}


struct tree *
parser_parse_program (struct state *state, struct parser *parser);

struct tree *
parser_parse_statement (struct state *state, struct parser *parser);

struct tree *
parser_parse_term (struct state *state, struct parser *parser);

struct tree *
parser_parse_assignment (struct state *state, struct parser *parser);

struct tree *
parser_parse_abstraction (struct state *state, struct parser *parser);

struct tree *
parser_parse_match (struct state *state, struct parser *parser);

struct tree *
parser_parse_match_arm (struct state *state, struct parser *parser);

struct tree *
parser_parse_pattern (struct state *state, struct parser *parser);

struct tree *
parser_parse_branch (struct state *state, struct parser *parser);

struct tree *
parser_parse_binary (struct state *state, struct parser *parser);

struct tree *
parser_parse_application (struct state *state, struct parser *parser);

struct tree *
parser_parse_atom (struct state *state, struct parser *parser);

struct tree *
parser_parse_group (struct state *state, struct parser *parser);

struct tree *
parser_parse_list (struct state *state, struct parser *parser);

struct tree *
parser_parse_compound (struct state *state, struct parser *parser);

struct tree *
parser_parse_integer (struct state *state, struct parser *parser);

struct tree *
parser_parse_float (struct state *state, struct parser *parser);

struct tree *
parser_parse_string (struct state *state, struct parser *parser);

struct tree *
parser_parse_name (struct state *state, struct parser *parser);


struct tree *
parser_parse_program (struct state *state, struct parser *parser)
{
  struct tree *result;

  result = tree_create (state, parser->current->location, TREE_PROGRAM);

  state_unhealthy_return2 (state, NULL);

  while (1)
    switch (parser->current->kind)
      {
      case TOKEN_EOF:
      case TOKEN_RBRACE:
        return result;
      default:
        {
          struct tree *statement;

          statement = parser_parse_statement (state, parser);

          state_unhealthy_return2 (state, NULL);

          tree_append (result, statement);
        }
        break;
      }
}


struct tree *
parser_parse_statement (struct state *state, struct parser *parser)
{
  struct tree *result;

  result = parser_parse_term (state, parser);

  state_unhealthy_return2 (state, NULL);

  parser_expect_kind (state, parser, TOKEN_SEMICOLON);

  state_unhealthy_return2 (state, NULL);

  parser_advance (state, parser);

  state_unhealthy_return2 (state, NULL);

  return result;
}


struct tree *
parser_parse_term (struct state *state, struct parser *parser)
{
  return parser_parse_assignment (state, parser);
}


struct tree *
parser_parse_assignment (struct state *state, struct parser *parser)
{
  if (parser_match_kind (parser, TOKEN_NAME))
    {
      struct token *peek = lexer_peek (state, parser->lexer);

      state_unhealthy_return2 (state, NULL);

      if (token_match (peek, TOKEN_EQUALS))
        {
          struct tree *left;

          left = parser_parse_name (state, parser);

          state_unhealthy_return2 (state, NULL);

          // Advance past the *known* EQUALS.
          parser_advance (state, parser);

          state_unhealthy_return2 (state, NULL);

          struct tree *right;

          right = parser_parse_assignment (state, parser);

          state_unhealthy_return2 (state, NULL);

          struct tree *result;

          result = tree_create (state, left->location, TREE_ASSIGNMENT);

          state_unhealthy_return2 (state, NULL);

          tree_append (result, left);
          tree_append (result, right);

          return result;
        }

      // Fallthrough.
    }

  return parser_parse_abstraction (state, parser);
}


struct tree *
parser_parse_abstraction (struct state *state, struct parser *parser)
{
  if (parser_match_kind (parser, TOKEN_NAME))
    {
      struct token *peek = lexer_peek (state, parser->lexer);

      state_unhealthy_return2 (state, NULL);

      if (token_match (peek, TOKEN_DOT))
        {
          struct tree *left;

          left = parser_parse_name (state, parser);

          state_unhealthy_return2 (state, NULL);

          // Advance past the *known* DOT.
          parser_advance (state, parser);

          state_unhealthy_return2 (state, NULL);

          struct tree *right;

          right = parser_parse_abstraction (state, parser);

          state_unhealthy_return2 (state, NULL);

          struct tree *result;

          result = tree_create (state, left->location, TREE_LAMBDA);

          state_unhealthy_return2 (state, NULL);

          tree_append (result, left);
          tree_append (result, right);

          return result;
        }

      // Fallthrough.
    }

  return parser_parse_branch (state, parser);
}


struct tree *
parser_parse_match (struct state *state, struct parser *parser)
{
  struct location location = parser->current->location;

  parser_expect_kind (state, parser, TOKEN_MATCH);

  state_unhealthy_return2 (state, NULL);

  parser_advance (state, parser);

  state_unhealthy_return2 (state, NULL);

  struct tree *subject;

  subject = parser_parse_binary (state, parser);

  state_unhealthy_return2 (state, NULL);

  struct tree *match;

  match = tree_create (state, location, TREE_MATCH);

  state_unhealthy_return2 (state, NULL);

  tree_append (match, subject);

  struct tree *arm;

  arm = parser_parse_match_arm (state, parser);

  state_unhealthy_return2 (state, NULL);

  tree_append (match, arm);

  return match;
}


struct tree *
parser_parse_match_arm (struct state *state, struct parser *parser)
{
  struct location location = parser->current->location;

  parser_expect_kind (state, parser, TOKEN_PIPE);

  state_unhealthy_return2 (state, NULL);

  parser_advance (state, parser);

  state_unhealthy_return2 (state, NULL);

  struct tree *pattern;

  pattern = parser_parse_pattern (state, parser);

  state_unhealthy_return2 (state, NULL);

  parser_expect_kind (state, parser, TOKEN_EQUALS);

  state_unhealthy_return2 (state, NULL);

  parser_advance (state, parser);

  state_unhealthy_return2 (state, NULL);

  struct tree *body;

  body = parser_parse_binary (state, parser);

  state_unhealthy_return2 (state, NULL);

  struct tree *arm;

  arm = tree_create (state, location, TREE_MATCH_ARM);

  state_unhealthy_return2 (state, NULL);

  tree_append (arm, pattern);
  tree_append (arm, body);

  if (parser_match_kind (parser, TOKEN_PIPE))
    {
      struct tree *consequent;

      consequent = parser_parse_match_arm (state, parser);

      state_unhealthy_return2 (state, NULL);

      tree_append (arm, consequent);
    }

  return arm;
}


// Recursively build (a : (b : ...))
struct tree *
parser_parse_pattern_list_tail (struct state *state, struct parser *parser)
{
  struct location location = parser->current->location;

  if (parser_match_kind (parser, TOKEN_RBRACKET))
    {
      parser_advance (state, parser);

      state_unhealthy_return2 (state, NULL);

      return tree_create (state, location, TREE_PATTERN_LIST_EMPTY);
    }

  struct tree *head;

  head = parser_parse_pattern (state, parser);

  state_unhealthy_return2 (state, NULL);

  struct tree *tail;

  if (parser_match_kind (parser, TOKEN_COMMA))
    parser_advance (state, parser);
  else
    parser_expect_kind (state, parser, TOKEN_RBRACKET);

  state_unhealthy_return2 (state, NULL);

  tail = parser_parse_pattern_list_tail (state, parser);

  state_unhealthy_return2 (state, NULL);

  struct tree *cons;

  cons = tree_create (state, location, TREE_PATTERN_LIST_CONS);

  state_unhealthy_return2 (state, NULL);

  tree_append (cons, head);
  tree_append (cons, tail);

  return cons;
}


struct tree *
parser_parse_pattern_list (struct state *state, struct parser *parser)
{
  parser_expect_kind (state, parser, TOKEN_LBRACKET);

  state_unhealthy_return2 (state, NULL);

  parser_advance (state, parser);

  state_unhealthy_return2 (state, NULL);

  return parser_parse_pattern_list_tail (state, parser);
}


struct tree *
parser_parse_pattern_atom (struct state *state, struct parser *parser)
{
  struct location location = parser->current->location;

  // Wildcard
  if (token_match_name (parser->current, "_"))
    {
      parser_advance (state, parser);

      state_unhealthy_return2 (state, NULL);

      return tree_create (state, location, TREE_PATTERN_WILDCARD);
    }

  // Unit or parenthesized pattern
  if (parser_match_kind (parser, TOKEN_LPAREN))
    {
      parser_advance (state, parser);

      state_unhealthy_return2 (state, NULL);

      // Unit
      if (parser_match_kind (parser, TOKEN_RPAREN))
        {
          parser_advance (state, parser);

          state_unhealthy_return2 (state, NULL);

          struct tree *atom;

          atom = tree_create (state, location, TREE_UNIT);

          state_unhealthy_return2 (state, NULL);

          struct tree *literal;

          literal = tree_create (state, location, TREE_PATTERN_LITERAL);

          state_unhealthy_return2 (state, NULL);

          tree_append (literal, atom);

          return literal;
        }

      struct tree *pattern;

      pattern = parser_parse_pattern (state, parser);

      state_unhealthy_return2 (state, NULL);

      parser_expect_kind (state, parser, TOKEN_RPAREN);

      state_unhealthy_return2 (state, NULL);

      parser_advance (state, parser);

      state_unhealthy_return2 (state, NULL);

      return pattern;
    }

  // Integer
  if (parser_match_kind (parser, TOKEN_INTEGER))
    {
      struct tree *atom;

      atom = parser_parse_integer (state, parser);

      state_unhealthy_return2 (state, NULL);

      struct tree *literal;

      literal = tree_create (state, location, TREE_PATTERN_LITERAL);

      state_unhealthy_return2 (state, NULL);

      tree_append (literal, atom);

      return literal;
    }

  // Float
  if (parser_match_kind (parser, TOKEN_FLOAT))
    {
      struct tree *atom;

      atom = parser_parse_float (state, parser);

      state_unhealthy_return2 (state, NULL);

      struct tree *literal;

      literal = tree_create (state, location, TREE_PATTERN_LITERAL);

      state_unhealthy_return2 (state, NULL);

      tree_append (literal, atom);

      return literal;
    }

  // String
  if (parser_match_kind (parser, TOKEN_STRING))
    {
      struct tree *atom;

      atom = parser_parse_string (state, parser);

      state_unhealthy_return2 (state, NULL);

      struct tree *literal;

      literal = tree_create (state, location, TREE_PATTERN_LITERAL);

      state_unhealthy_return2 (state, NULL);

      tree_append (literal, atom);

      return literal;
    }

  // Name
  if (parser_match_kind (parser, TOKEN_NAME))
    {
      struct tree *atom;

      atom = parser_parse_name (state, parser);

      state_unhealthy_return2 (state, NULL);

      struct tree *name;

      name = tree_create (state, location, TREE_PATTERN_NAME);

      state_unhealthy_return2 (state, NULL);

      tree_append (name, atom);

      return name;
    }

  // List cons
  if (parser_match_kind (parser, TOKEN_LBRACKET))
    {
      return parser_parse_pattern_list (state, parser);
    }

  const char *b = token_kind_string (parser->current->kind);

  parser_expect (state, parser, "pattern", b);

  return NULL;
}


struct tree *
parser_parse_pattern (struct state *state, struct parser *parser)
{
  struct location location = parser->current->location;

  struct tree *lhs;

  lhs = parser_parse_pattern_atom (state, parser);

  state_unhealthy_return2 (state, NULL);

  if (token_match_name (parser->current, ":"))
    {
      parser_advance (state, parser);

      state_unhealthy_return2 (state, NULL);

      struct tree *rhs;

      rhs = parser_parse_pattern (state, parser);

      state_unhealthy_return2 (state, NULL);

      struct tree *cons;

      cons = tree_create (state, location, TREE_PATTERN_LIST_CONS);

      state_unhealthy_return2 (state, NULL);

      tree_append (cons, lhs);
      tree_append (cons, rhs);

      return cons;
    }

  return lhs;
}


struct tree *
parser_parse_branch (struct state *state, struct parser *parser)
{
  if (parser_match_kind (parser, TOKEN_MATCH))
    return parser_parse_match (state, parser);

  if (parser_match_kind (parser, TOKEN_PIPE))
    {
      struct location location = parser->current->location;

      parser_advance (state, parser);

      state_unhealthy_return2 (state, NULL);

      struct tree *condition;

      condition = parser_parse_binary (state, parser);

      state_unhealthy_return2 (state, NULL);

      parser_expect_kind (state, parser, TOKEN_EQUALS);

      state_unhealthy_return2 (state, NULL);

      parser_advance (state, parser);

      state_unhealthy_return2 (state, NULL);

      struct tree *consequent;

      consequent = parser_parse_binary (state, parser);

      state_unhealthy_return2 (state, NULL);

      struct tree *result;

      result = tree_create (state, location, TREE_BRANCH);

      state_unhealthy_return2 (state, NULL);

      tree_append (result, condition);
      tree_append (result, consequent);

      if (parser_match_kind (parser, TOKEN_PIPE))
        {
          struct tree *alternative;

          alternative = parser_parse_branch (state, parser);

          state_unhealthy_return2 (state, NULL);

          tree_append (result, alternative);
        }

      return result;
    }

  return parser_parse_binary (state, parser);
}


struct tree *
parser_parse_binary_base (struct state *state, struct parser *parser, int p)
{
  struct tree *left;

  left = parser_parse_application (state, parser);

  state_unhealthy_return2 (state, NULL);

  struct operator operator;

  operator = parser_fetch_operator (parser->current);

  if (operator.s == NULL)
    return left;

  while ((operator.a == ASSOCIATIVITY_L && operator.p > p) ||
         (operator.a == ASSOCIATIVITY_R && operator.p >= p))
    {
      struct tree *function;

      function = parser_parse_name (state, parser);

      state_unhealthy_return2 (state, NULL);

      struct tree *right;

      if (operator.a == ASSOCIATIVITY_L)
        right = parser_parse_binary_base (state, parser, operator.p);
      else
        right = parser_parse_binary_base (state, parser, operator.p - 1);

      state_unhealthy_return2 (state, NULL);

      struct tree *call;

      call = tree_create (state, function->location, TREE_CALL_BINARY);

      tree_append (call, function);
      tree_append (call, left);
      tree_append (call, right);

      left = call;

      /*
      struct tree *a1;
      struct tree *a2;

      // NOTE: ((function left) right)

      a1 = tree_create (state, function->location, TREE_CALL);

      state_unhealthy_return2 (state, NULL);

      tree_append (a1, function);
      tree_append (a1, left);

      a2 = tree_create (state, function->location, TREE_CALL);

      state_unhealthy_return2 (state, NULL);

      tree_append (a2, a1);
      tree_append (a2, right);

      left = a2;
      */

      operator = parser_fetch_operator (parser->current);

      if (operator.s == NULL)
        return left;
    }

  return left;
}


struct tree *
parser_parse_binary (struct state *state, struct parser *parser)
{
  return parser_parse_binary_base (state, parser, 0);
}


bool
parser_is_application_terminator (struct token *token)
{
  // Tokens which terminate a function application.
  switch (token->kind)
    {
    case TOKEN_NAME:
      // Any infix operator.
      return parser_fetch_operator (token).p > 0;
    case TOKEN_EOF:
    case TOKEN_EQUALS:
    case TOKEN_SEMICOLON:
    case TOKEN_COMMA:
    case TOKEN_DOT:
    case TOKEN_PIPE:
    case TOKEN_RPAREN:
    case TOKEN_RBRACKET:
    case TOKEN_RBRACE:
      return true;
    default:
      return false;
    }
}


struct tree *
parser_parse_application (struct state *state, struct parser *parser)
{
  struct tree *left;

  left = parser_parse_atom (state, parser);

  state_unhealthy_return2 (state, NULL);

  while (!parser_is_application_terminator (parser->current))
    {
      struct tree *right;

      right = parser_parse_atom (state, parser);

      state_unhealthy_return2 (state, NULL);

      struct tree *result;

      result = tree_create (state, left->location, TREE_CALL);

      state_unhealthy_return2 (state, NULL);

      tree_append (result, left);
      tree_append (result, right);

      left = result;
    }

  return left;
}


struct tree *
parser_parse_atom_base (struct state *state, struct parser *parser, enum token_kind token_kind,
                        enum tree_kind tree_kind)
{
  parser_expect_kind (state, parser, token_kind);

  state_unhealthy_return2 (state, NULL);

  struct tree *result;

  result = tree_create (state, parser->current->location, tree_kind);

  state_unhealthy_return2 (state, NULL);

  result->token = parser->current;

  parser_advance (state, parser);

  state_unhealthy_return2 (state, NULL);

  return result;
}


struct tree *
parser_parse_atom (struct state *state, struct parser *parser)
{
  switch (parser->current->kind)
    {
    case TOKEN_LPAREN:
      return parser_parse_group (state, parser);
    case TOKEN_LBRACKET:
      return parser_parse_list (state, parser);
    case TOKEN_LBRACE:
      return parser_parse_compound (state, parser);
    case TOKEN_INTEGER:
      return parser_parse_integer (state, parser);
    case TOKEN_FLOAT:
      return parser_parse_float (state, parser);
    case TOKEN_STRING:
      return parser_parse_string (state, parser);
    case TOKEN_NAME:
      return parser_parse_name (state, parser);
    default:
      break;
    }

  const char *b = token_kind_string (parser->current->kind);

  parser_expect (state, parser, "expression", b);

  return NULL;
}


struct tree *
parser_parse_group (struct state *state, struct parser *parser)
{
  struct location location = parser->current->location;

  parser_expect_kind (state, parser, TOKEN_LPAREN);

  state_unhealthy_return2 (state, NULL);

  parser_advance (state, parser);

  state_unhealthy_return2 (state, NULL);

  struct tree *result;

  // ()
  if (parser_match_kind (parser, TOKEN_RPAREN))
    {
      parser_advance (state, parser);

      state_unhealthy_return2 (state, NULL);

      result = tree_create (state, location, TREE_UNIT);

      state_unhealthy_return2 (state, NULL);

      return result;
    }

  result = parser_parse_term (state, parser);

  state_unhealthy_return2 (state, NULL);

  result->location = location;

  parser_expect_kind (state, parser, TOKEN_RPAREN);

  state_unhealthy_return2 (state, NULL);

  parser_advance (state, parser);

  state_unhealthy_return2 (state, NULL);

  return result;
}


struct tree *
parser_parse_list (struct state *state, struct parser *parser)
{
  struct location location = parser->current->location;

  parser_expect_kind (state, parser, TOKEN_LBRACKET);

  state_unhealthy_return2 (state, NULL);

  parser_advance (state, parser);

  state_unhealthy_return2 (state, NULL);

  struct tree *result;

  result = tree_create (state, location, TREE_LIST);

  state_unhealthy_return2 (state, NULL);

  while (!parser_match_kind (parser, TOKEN_RBRACKET))
    {
      struct tree *value;

      value = parser_parse_binary (state, parser);

      state_unhealthy_return2 (state, NULL);

      tree_append (result, value);

      if (parser_match_kind (parser, TOKEN_COMMA))
        parser_advance (state, parser);
      else
        parser_expect_kind (state, parser, TOKEN_RBRACKET);

      state_unhealthy_return2 (state, NULL);
    }

  // Advance past the *known* RBRACKET.
  parser_advance (state, parser);

  state_unhealthy_return2 (state, NULL);

  return result;
}


struct tree *
parser_parse_compound (struct state *state, struct parser *parser)
{
  struct location location = parser->current->location;

  parser_expect_kind (state, parser, TOKEN_LBRACE);

  state_unhealthy_return2 (state, NULL);

  parser_advance (state, parser);

  state_unhealthy_return2 (state, NULL);

  // NOTE: Compound statement cannot be emtpy.
  if (parser_match_kind (parser, TOKEN_RBRACE))
    {
      const char *b = token_kind_string (parser->current->kind);

      parser_expect (state, parser, "statement", b);

      return NULL;
    }

  struct tree *result;

  result = parser_parse_program (state, parser);

  state_unhealthy_return2 (state, NULL);

  result->location = location;

  parser_expect_kind (state, parser, TOKEN_RBRACE);

  state_unhealthy_return2 (state, NULL);

  parser_advance (state, parser);

  state_unhealthy_return2 (state, NULL);

  return result;
}


struct tree *
parser_parse_integer (struct state *state, struct parser *parser)
{
  return parser_parse_atom_base (state, parser, TOKEN_INTEGER, TREE_INTEGER);
}


struct tree *
parser_parse_float (struct state *state, struct parser *parser)
{
  return parser_parse_atom_base (state, parser, TOKEN_FLOAT, TREE_FLOAT);
}


struct tree *
parser_parse_string (struct state *state, struct parser *parser)
{
  return parser_parse_atom_base (state, parser, TOKEN_STRING, TREE_STRING);
}


struct tree *
parser_parse_name (struct state *state, struct parser *parser)
{
  return parser_parse_atom_base (state, parser, TOKEN_NAME, TREE_NAME);
}


struct tree *
parser_parse (struct state *state, struct parser *parser)
{
  parser_advance (state, parser);

  state_unhealthy_return2 (state, NULL);

  struct tree *result = parser_parse_program (state, parser);

  state_unhealthy_return2 (state, NULL);

  parser_expect_kind (state, parser, TOKEN_EOF);

  state_unhealthy_return2 (state, NULL);

  parser_advance (state, parser);

  state_unhealthy_return2 (state, NULL);

  return result;
}

