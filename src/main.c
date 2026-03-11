#include <string.h>
#include <errno.h>

#include "state.h"

#include "compiler.h"
#include "builtin.h"
#include "lambda.h"
#include "gc.h"
#include "parser.h"
#include "scope.h"
#include "value.h"
#include "program.h"
#include "native.h"
#include "vm.h"


////////////////////////////////////////////////////////////////////////////////////////////////////



static FILE *
file_open (struct state *state, const char *path, const char *mode)
{
  FILE *file = fopen (path, mode);

  if (file == NULL)
    {
      state_set_error (state, "%s: %s", path, strerror (errno));

      return NULL;
    }

  return file;
}


static void
file_close (FILE *file)
{
  fclose (file);
}


static char *
file_read (struct state *state, const char *path)
{
  FILE *file = file_open (state, path, "r");

  state_unhealthy_return2 (state, NULL);

  fseek (file, 0, SEEK_END);

  size_t file_size = ftell (file);

  fseek (file, 0, SEEK_SET);

  char *buffer = state_aa_malloc (state, file_size + 1);

  if (state->unhealthy)
    {
      file_close (file);

      return NULL;
    }

  (void)fread (buffer, 1, file_size, file);

  buffer[file_size] = '\0';

  file_close (file);

  return buffer;
}


////////////////////////////////////////////////////////////////////////////////////////////////////


bool
check_unhealthy (struct state *state, const char *phase)
{
  if (!state->unhealthy)
    return false;

  struct error e = state->error;

  struct location l = state->location;

  if (l.context)
    fprintf (stderr, "\033[94m%s:%zu:%zu\033[0m: ", l.context, l.line, l.column);

  fprintf (stderr, "\033[91mERROR\033[0m: %s [\033[93m%s\033[0m]\n", e.message, phase);

  return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////


s32
main (s32 argc, char **argv)
{
  bool flag_A = false;
  bool flag_S = false;
  const char *file = NULL;

  for (int i = 1; i < argc; i++)
    {
      if (strcmp(argv[i], "-A") == 0)
        flag_A = true;
      else if (strcmp(argv[i], "-S") == 0)
        flag_S = true;
      else
        file = argv[i];
    }

  if (file == NULL)
    {
      fprintf (stderr, "no input file\n");
      return 0;
    }

  usize allocs1 = 0, frees1 = 0;
  usize allocs2 = 0, frees2 = 0;

  usize frames_len_record = 0;
  usize runs = 0;

  struct state *state = state_create ();

  const char *text = file_read (state, file);

  if (check_unhealthy (state, "PARSER"))
    goto clean_compile_time;

  // Comptime
  struct parser *parser = parser_create (state, file, text);

  struct tree *tree = parser_parse (state, parser);

  if (check_unhealthy (state, "PARSER"))
    goto clean_compile_time;

  if (flag_A)
    {
      tree_debug_print (tree);
      goto clean_compile_time;
    }

  struct program *program = compiler_compile (state, tree);

  if (check_unhealthy (state, "COMPILER"))
    goto clean_compile_time;

  if (flag_S)
    {
      fprintf (stderr, "\n");

      program_debug_print (program, 2);

      fprintf (stderr, "\n");
      fprintf (stderr, "\n");

      program_destroy (state, program);

      goto clean_compile_time;
    }

  // Runtime
  struct scope *scope;

  scope = scope_create (state, NULL, 64);

  builtin_set (state, scope);

  struct frame *frame;

  frame = frame_create (state, program, scope);

  struct vm *vm;

  vm = vm_create (state, 1024);

  vm_push (state, vm, frame);

  allocs1 = state->allocs;
  frees1  = state->frees;

  vm_execute (state, vm);

  allocs2 = state->allocs;
  frees2  = state->frees;

  frames_len_record = vm->frames_len_record;
  runs = state->gc->runs;

  if (check_unhealthy (state, "VM"))
    goto clean_run_time;

  frame_destroy (state, frame);

clean_run_time:
  vm_destroy (state, vm);

  program_destroy (state, program);

clean_compile_time:
  state_destroy (state);

  usize allocs = allocs2 - allocs1;
  usize frees  = frees2  - frees1;
  isize delta  = allocs - frees;

  fprintf (stderr, "________________________________________________________\n");
  fprintf (stderr, "Finished; %zu allocs, %zu frees, Δ = %+zd,\n", allocs, frees, delta);
  fprintf (stderr, "          %zu stack frames,\n", frames_len_record);
  fprintf (stderr, "          %zu GC runs,\n", runs);

  return 0;
}

