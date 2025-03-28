#include <stdio.h>
#include <stdlib.h>

#include "arena.h"
#include "array.h"
#include "ast.h"
#include "error.h"
#include "evaluator.h"
// #include "function.h"
#include "lexer.h"
#include "native.h"
#include "parser.h"
#include "standard.h"
#include "string.h"
#include "table.h"
// #include "thunk.h"
#include "token.h"
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

  (void)fread (buffer, 1, file_size, file);

  buffer[file_size] = '\0';

  fclose (file);
  return buffer;
}

void
table_append_native (struct table *table, native_c_function_t f, const char *k)
{
  struct value *value = value_create (VALUE_NATIVE, (struct location) { 0 });
  value->value.native = native_create (f);
  table_append (table, table_entry_create (k, value));
  value_destroy (value);
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
  // else
  //   ast_debug_print (ast, 0);

  struct table *gst = table_create (4, NULL);

  table_append_native (gst, standard_f_f, "_f");

  table_append_native (gst, standard_f_print, "print");
  table_append_native (gst, standard_f_printl, "printl");

  table_append_native (gst, standard_f_add, "+");
  table_append_native (gst, standard_f_sub, "-");
  table_append_native (gst, standard_f_mul, "*");
  table_append_native (gst, standard_f_div, "/");
  table_append_native (gst, standard_f_mod, "%");

  table_append_native (gst, standard_f_eq, "==");
  table_append_native (gst, standard_f_neq, "!=");

  table_append_native (gst, standard_f_head, "head");
  table_append_native (gst, standard_f_tail, "tail");
  table_append_native (gst, standard_f_push_front, ">>");

  table_append_native (gst, standard_f_range, "range");

  table_append_native (gst, standard_f_map, "map");
  table_append_native (gst, standard_f_map_, "map_");

  table_append_native (gst, standard_f_error, "error");

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

  // printf ("----------------\n");
  // printf ("evaluated type:  %s\n",
  //         value_type_string (value->type));
  // printf ("evaluated value: ");
  // value_debug_print (value);
  // printf ("\n");

  arena_destroy (&lexer_arena);
  arena_destroy (&parser_arena);

  // printf ("-------------------\n");
  // table_debug_print (gst);

  free (src);
  value_destroy (value);
  table_destroy (gst);

  return 0;
}

