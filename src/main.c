#include <stdio.h>
#include <stdlib.h>

#include "evaluator.h"
#include "table.h"
#include "arena.h"
#include "ast.h"
#include "parser.h"
#include "value.h"

static char *
read_file (const char *filename)
{
  FILE *file = fopen (filename, "r");
  if (file == NULL)
    {
      perror ("Cannnot open file");
      return NULL;
    }

  fseek (file, 0, SEEK_END);
  size_t file_size = ftell (file);
  fseek (file, 0, SEEK_SET);

  char *buffer = (char *)malloc (file_size + 1);

  size_t bytes_read = fread (buffer, 1, file_size, file);

  buffer[file_size] = '\0';

  fclose (file);
  return buffer;
}

int
main (int argc, char *argv[])
{
  if (argc != 2)
    {
      fprintf (stderr, "Usage: %s <file>\n", argv[0]);
      abort ();
    }

  char *src = read_file (argv[1]);

  struct arena lexer_arena = {0};
  struct lexer lexer = lexer_create (src, argv[1], &lexer_arena);

  struct arena parser_arena = {0};
  struct parser parser = parser_create (&lexer, &parser_arena);

  struct ast *ast = parser_parse (&parser);

  if (ast_match_error (ast))
    {
      location_debug_print (ast->location);
      printf (": fatal-error: %s [SYNTAX]\n", ast->value.error.message);

      arena_destroy (&lexer_arena);
      arena_destroy (&parser_arena);

      free (src);
      exit (1);
    }
  else
    ast_debug_print (ast, 0);

  struct table *gst = table_create (4, NULL);
  struct value *value = evaluate (ast, gst);

  if (value_match_error (value))
    {
      location_debug_print (value->location);
      printf (": fatal-error: %s [RUNTIME]\n", value->value.error.message);

      arena_destroy (&lexer_arena);
      arena_destroy (&parser_arena);

      value_destroy (value);
      table_destroy (gst);

      free (src);
      exit (1);
    }

  printf ("evaluated type:  %s\n",
          value_type_string (value->type));
  printf ("evaluated value: ");
  value_debug_print (value);
  printf ("\n");

  arena_destroy (&lexer_arena);
  arena_destroy (&parser_arena);

  for (size_t i = 0; i < gst->size; ++i)
    {
      printf ("%s=", gst->storage[i]->key);
      value_debug_print (gst->storage[i]->value);
      printf ("\n");
    }

  free (src);
  value_destroy (value);
  table_destroy (gst);

  /*struct parser parser = parser_create ("a + b +", "__tmp__");
  struct ast *ast = parser_parse (&parser);

  if (ast_match_error (ast))
    {
      location_debug_print (ast->location);
      printf (": fatal-error: %s\n", ast->value.error.message);
      ast_destroy (ast);
      // parser_cleanup (&parser);
      exit (1);
    }

  ast_debug_print (ast, 0);
  ast_destroy (ast);
  */

  /*
  struct lexer lexer = lexer_create ("a + b", "<>", NULL);
  struct token token;

  while (1)
    {
      token = lexer_next (&lexer);
      if (token.type == TOKEN_NOTHING || token.type == TOKEN_ERROR)
        break;

      printf ("'%s'", token_type_string (token.type));
      switch (token.type)
        {
        case TOKEN_IDENTIFIER:
          printf (": `%s`", token.value.s);
          break;
        case TOKEN_STRING:
          printf (": \"%s\"", token.value.s);
          break;
        case TOKEN_INTEGER:
          printf (": %ld", token.value.i);
          break;
        case TOKEN_FLOAT:
          printf (": %lf", token.value.f);
          break;
        default:
          break;
        }

      token_destroy (token);
      printf ("\n");
    }
  */

  return 0;
}

