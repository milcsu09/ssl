#include "ast.h"
#include "parser.h"
#include <stdlib.h>
#include <string.h>

struct parser
parser_create (struct lexer *lexer, struct arena *arena)
{
  struct parser parser;

  parser.lexer = lexer;
  parser.arena = arena;

  return parser;
}

struct precedence
{
  const char *key;
  int precedence;
  int side;
};

static struct precedence PRECEDENCE_TABLE[] = {
  /* LATER.
   * { "||",  5 },
   * { "&&",  10 },

   * { "|",   15 },
   * { "^",   15 },
   * { "&",   15 },

   * { "==",  20 },
   * { "!=",  20 },

   * { "<",   25 },
   * { ">",   25 },
   * { "<=",  25 },
   * { ">=",  25 },*/

  { "$",    5, 1 },
  { ".",    5, 1 },

  { "==",  20, 0 },
  { "!=",  20, 0 },

  { "<",   25, 0 },
  { ">",   25, 0 },
  { "<=",  25, 0 },
  { ">=",  25, 0 },

  { ">>",  30, 1 },

  { "+",   35, 0 },
  { "-",   35, 0 },

  { "*",   40, 0 },
  { "/",   40, 0 },
  { "%",   40, 0 },
};

static struct precedence
parser_query_precedence_table (struct token token)
{
  if (!token_match (token, TOKEN_IDENTIFIER))
    goto invalid;

  size_t size = sizeof (PRECEDENCE_TABLE) / sizeof (PRECEDENCE_TABLE[0]);

  for (size_t i = 0; i < size; ++i)
    {
      struct precedence current = PRECEDENCE_TABLE[i];

      if (strcmp (current.key, token.value.s) == 0)
        return current;
    }

invalid:
  return (struct precedence) { 0 };
}

static int
parser_is_delimiter (struct token token)
{
  switch (token.type)
    {
    case TOKEN_IDENTIFIER:
      /* Mark tokens that have non-zero precedence as delimiters. This allows
       * expressions like `f x + g x`, where '+' has a non-zero precedence, to
       * become `+ (f x) (g x)` instead of `f (x) (+) (g) (x)` */
      return parser_query_precedence_table (token).precedence > 0;
    case TOKEN_NOTHING:
    case TOKEN_SEMICOLON:
    case TOKEN_RPAREN:
    case TOKEN_RBRACKET:
    case TOKEN_RBRACE:
    case TOKEN_COMMA:
    case TOKEN_PIPE:
    case TOKEN_QUESTION:
      return 1;
    default:
      return 0;
    }
}

static int
parser_match (struct parser *parser, enum token_type type)
{
  return token_match (parser->current, type);
}

static int
parser_match_error (struct parser *parser)
{
  return parser_match (parser, TOKEN_ERROR);
}

static struct ast *
parser_error_from_token (struct parser *parser, struct token token,
                         struct location location)
{
  return ast_create_e (token.value.error, location, parser->arena);
}

static struct ast *
parser_error_from_current (struct parser *parser)
{
  return parser_error_from_token (parser, parser->current, parser->location);
}

static struct ast *
parser_error_expect_base (struct parser *parser, const char *a, const char *b)
{
  return ast_create_e (error_create ("expected `%s`, got `%s`", a, b),
                       parser->location, parser->arena);
}

static struct ast *
parser_error_expect_token (struct parser *parser, enum token_type type)
{
  const char *a = token_type_string (type);
  const char *b = token_type_string (parser->current.type);
  return parser_error_expect_base (parser, a, b);
}

static int
parser_advance (struct parser *parser)
{
  /* 'lexer_next' might return token of type 'TOKEN_ERROR'. */
  parser->current = lexer_next (parser->lexer);
  parser->location = parser->current.location;
  return parser_match_error (parser);
}

static struct ast *
parser_parse_atom (struct parser *parser, enum ast_type type,
                   enum token_type token)
{
  if (!parser_match (parser, token))
    return parser_error_expect_token (parser, token);

  struct ast *result;

  result = ast_create (type, parser->location, parser->arena);
  result->value.token = parser->current;

  if (parser_advance (parser))
    return parser_error_from_current (parser);

  return result;
}

static struct ast *parser_parse_statement (struct parser *);

static struct ast *parser_parse_expression3 (struct parser *);
static struct ast *parser_parse_expression2 (struct parser *);
static struct ast *parser_parse_expression1 (struct parser *);
static struct ast *parser_parse_expression0 (struct parser *);

static struct ast *parser_parse_identifier (struct parser *);
static struct ast *parser_parse_string (struct parser *);
static struct ast *parser_parse_integer (struct parser *);
static struct ast *parser_parse_float (struct parser *);
static struct ast *parser_parse_array (struct parser *);
static struct ast *parser_parse_grouping (struct parser *);
static struct ast *parser_parse_compound_expression (struct parser *);
static struct ast *parser_parse_variable_assignment (struct parser *);
static struct ast *parser_parse_function_definition (struct parser *);
// Later.
// static struct ast *parser_parse_turnary (struct parser *);
// static struct ast *parser_parse_where (struct parser *);
static struct ast *parser_parse_program (struct parser *);

static struct ast *
parser_parse_statement (struct parser *parser)
{
  struct ast *result;

  result = parser_parse_expression3 (parser);
  if (ast_match_error (result))
    return result;

  if (!parser_match (parser, TOKEN_SEMICOLON))
    return parser_error_expect_token (parser, TOKEN_SEMICOLON);

  if (parser_advance (parser))
    return parser_error_from_current (parser);

  return result;
}

static struct ast *
parser_parse_expression3 (struct parser *parser)
{
  if (token_match (parser->current, TOKEN_PIPE))
    {
      if (parser_advance (parser))
        return parser_error_from_current (parser);

      struct ast *condition;
      struct ast *expression1;
      struct ast *expression2;

      condition = parser_parse_expression2 (parser);
      if (ast_match_error (condition))
        return condition;

      if (!parser_match (parser, TOKEN_QUESTION))
        return parser_error_expect_token (parser, TOKEN_QUESTION);

      if (parser_advance (parser))
        return parser_error_from_current (parser);

      expression1 = parser_parse_expression2 (parser);
      if (ast_match_error (expression1))
        return expression1;

      struct ast *result;

      result = ast_create (AST_TURNARY, parser->current.location,
                           parser->arena);

      ast_append (result, condition);
      ast_append (result, expression1);

      if (!token_match (parser->current, TOKEN_PIPE))
        return result;

      expression2 = parser_parse_expression3 (parser);
      if (ast_match_error (expression2))
        return expression2;

      ast_append (result, expression2);

      return result;
    }
  else
    return parser_parse_expression2 (parser);

  /*
  if (parser->current.type == TOKEN_PIPE)
    {
      parser_advance (parser);

      struct ast *result = ast_create (AST_GUARD, parser->current.line);

      struct ast *cond = parser_parse_expression0 (parser, 0);
      parser_match_advance (parser, TOKEN_QMARK);
      struct ast *expr1 = parser_parse_expression0 (parser, 0);

      ast_append (result, cond);
      ast_append (result, expr1);

      if (parser->current.type != TOKEN_PIPE)
        return result;

      struct ast *expr2 = parser_parse_expression_f (parser);
      ast_append (result, expr2);

      return result;
    }
  else
    return parser_parse_expression0 (parser, 0);
    */
}

static struct ast *
parser_parse_expression2_base (struct parser *parser, int previous)
{
  struct ast *left;

  left = parser_parse_expression1 (parser);
  if (ast_match_error (left))
    return left;

  struct precedence p;
  p = parser_query_precedence_table (parser->current);

  while ((p.side == 0 && p.precedence > previous) ||
         (p.side == 1 && p.precedence >= previous))
    {
      struct ast *middle;

      middle = parser_parse_expression0 (parser);
      if (ast_match_error (middle))
        return middle;

      struct ast *right;

      if (p.side == 0)
        right = parser_parse_expression2_base (parser, p.precedence);
      else
        right = parser_parse_expression2_base (parser, p.precedence - 1);

      if (ast_match_error (right))
        return right;

      struct ast *t1, *t2;

      /* Mimic currying. Generated AST is ((middle left) right)! */
      t1 = ast_create (AST_FUNCTION_INVOCATION, middle->location,
                       parser->arena);
      ast_append (t1, middle);
      ast_append (t1, left);

      t2 = ast_create (AST_FUNCTION_INVOCATION, middle->location,
                       parser->arena);
      ast_append (t2, t1);
      ast_append (t2, right);

      left = t2;

      p = parser_query_precedence_table (parser->current);
    }

  return left;
}

/* Wrapper function to call base function with default argument.  */
static struct ast *
parser_parse_expression2 (struct parser *parser)
{
  return parser_parse_expression2_base (parser, 0);
}

static struct ast *
parser_parse_expression1 (struct parser *parser)
{
  struct ast *left;

  left = parser_parse_expression0 (parser);
  if (ast_match_error (left))
    return left;

  while (!parser_is_delimiter (parser->current))
    {
      struct ast *right;

      right = parser_parse_expression0 (parser);
      if (ast_match_error (right))
        return right;

      struct ast *result;

      result = ast_create (AST_FUNCTION_INVOCATION, right->location,
                           parser->arena);
      ast_append (result, left);
      ast_append (result, right);
      left = result;
    }

  return left;
}

static struct ast *
parser_parse_expression0 (struct parser *parser)
{
  struct ast *result;

  switch (parser->current.type)
    {
    case TOKEN_IDENTIFIER:
      {
        struct token peek = lexer_peek (parser->lexer);

        /* Peek may leak memory. NOTE: `parser->lexer->arena` cleans it up. */

        if (token_match_error (peek))
          return parser_error_from_token (parser, peek, peek.location);

        if (token_match (peek, TOKEN_EQUAL))
          result = parser_parse_variable_assignment (parser);
        else if (token_match (peek, TOKEN_ARROW))
          result = parser_parse_function_definition (parser);
        else
          result = parser_parse_identifier (parser);
      }
      break;
    case TOKEN_STRING:
      result = parser_parse_string (parser);
      break;
    case TOKEN_INTEGER:
      result = parser_parse_integer (parser);
      break;
    case TOKEN_FLOAT:
      result = parser_parse_float (parser);
      break;
    case TOKEN_LPAREN:
      result = parser_parse_grouping (parser);
      break;
    case TOKEN_LBRACKET:
      result = parser_parse_array (parser);
      break;
    case TOKEN_LBRACE:
      result = parser_parse_compound_expression (parser);
      break;
    default:
      {
        const char *b = token_type_string (parser->current.type);
        return parser_error_expect_base (parser, "expression", b);
      }
      break;
    }

  return result;
}

static struct ast *
parser_parse_identifier (struct parser *parser)
{
  return parser_parse_atom (parser, AST_IDENTIFIER, TOKEN_IDENTIFIER);
}

static struct ast *
parser_parse_string (struct parser *parser)
{
  return parser_parse_atom (parser, AST_STRING, TOKEN_STRING);
}

static struct ast *
parser_parse_integer (struct parser *parser)
{
  return parser_parse_atom (parser, AST_INTEGER, TOKEN_INTEGER);
}

static struct ast *
parser_parse_float (struct parser *parser)
{
  return parser_parse_atom (parser, AST_FLOAT, TOKEN_FLOAT);
}

static struct ast *
parser_parse_array (struct parser *parser)
{
  if (!parser_match (parser, TOKEN_LBRACKET))
    return parser_error_expect_token (parser, TOKEN_LBRACKET);

  if (parser_advance (parser))
    return parser_error_from_current (parser);

  struct ast *result;

  result = ast_create (AST_ARRAY, parser->location, parser->arena);

  while (!parser_match (parser, TOKEN_RBRACKET)
         && !parser_match (parser, TOKEN_NOTHING))
    {
      struct ast *expression;

      expression = parser_parse_expression3 (parser);
      if (ast_match_error (expression))
        return expression;

      ast_append (result, expression);

      if (parser_match (parser, TOKEN_RBRACKET))
        break;

      if (!parser_match (parser, TOKEN_COMMA))
        return parser_error_expect_token (parser, TOKEN_COMMA);

      if (parser_advance (parser))
        return parser_error_from_current (parser);
    }

  if (!parser_match (parser, TOKEN_RBRACKET))
    return parser_error_expect_token (parser, TOKEN_RBRACKET);

  if (parser_advance (parser))
    return parser_error_from_current (parser);

  return result;
}

static struct ast *
parser_parse_grouping (struct parser *parser)
{
  if (!parser_match (parser, TOKEN_LPAREN))
    return parser_error_expect_token (parser, TOKEN_LPAREN);

  if (parser_advance (parser))
    return parser_error_from_current (parser);

  struct ast *result;

  result = parser_parse_expression3 (parser);
  if (ast_match_error (result))
    return result;

  if (!parser_match (parser, TOKEN_RPAREN))
    return parser_error_expect_token (parser, TOKEN_RPAREN);

  if (parser_advance (parser))
    return parser_error_from_current (parser);

  return result;
}

static struct ast *
parser_parse_compound_expression (struct parser *parser)
{
  if (!parser_match (parser, TOKEN_LBRACE))
    return parser_error_expect_token (parser, TOKEN_LBRACE);

  if (parser_advance (parser))
    return parser_error_from_current (parser);

  struct ast *result;

  result = parser_parse_program (parser);
  if (ast_match_error (result))
    return result;

  if (!parser_match (parser, TOKEN_RBRACE))
    return parser_error_expect_token (parser, TOKEN_RBRACE);

  if (parser_advance (parser))
    return parser_error_from_current (parser);

  return result;
}

static struct ast *
parser_parse_variable_assignment (struct parser *parser)
{
  struct ast *identifier;

  identifier = parser_parse_identifier (parser);
  if (ast_match_error (identifier))
    return identifier;

  if (!parser_match (parser, TOKEN_EQUAL))
    return parser_error_expect_token (parser, TOKEN_EQUAL);

  if (parser_advance (parser))
    return parser_error_from_current (parser);

  struct ast *expression;

  expression = parser_parse_expression3 (parser);
  if (ast_match_error (expression))
    return expression;

  struct ast *result;

  result = ast_create (AST_VARIABLE_ASSIGNMENT, identifier->location,
                       parser->arena);
  ast_append (result, identifier);
  ast_append (result, expression);

  return result;
}

static struct ast *
parser_parse_function_definition (struct parser *parser)
{
  struct ast *argument;

  argument = parser_parse_identifier (parser);
  if (ast_match_error (argument))
    return argument;

  if (!parser_match (parser, TOKEN_ARROW))
    return parser_error_expect_token (parser, TOKEN_ARROW);

  if (parser_advance (parser))
    return parser_error_from_current (parser);

  struct ast *body;

  body = parser_parse_expression3 (parser);
  if (ast_match_error (body))
    return body;

  struct ast *result;

  result = ast_create (AST_FUNCTION_DEFINITION, argument->location,
                       parser->arena);
  ast_append (result, argument);
  ast_append (result, body);

  return result;
}

static struct ast *
parser_parse_program (struct parser *parser)
{
  struct ast *result;

  result = ast_create (AST_PROGRAM, parser->location, parser->arena);

  while (1)
    switch (parser->current.type)
      {
      case TOKEN_RBRACE:
      case TOKEN_NOTHING:
        return result;
      default:
        {
          struct ast *statement;

          statement = parser_parse_statement (parser);
          if (ast_match_error (statement))
            return statement;

          ast_append (result, statement);
        }
        break;
      }
}

struct ast *
parser_parse (struct parser *parser)
{
  if (parser_advance (parser))
    return parser_error_from_current (parser);

  struct ast *result;

  result = parser_parse_program (parser);
  if (ast_match_error (result))
    return result;

  if (!parser_match (parser, TOKEN_NOTHING))
    return parser_error_expect_token (parser, TOKEN_NOTHING);

  if (parser_advance (parser))
    return parser_error_from_current (parser);

  return result;
}

