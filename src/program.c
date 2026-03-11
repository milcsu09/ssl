#include "program.h"
#include "value.h"


struct program *
program_create (struct state *state)
{
  struct program *program;

  program = state_malloc (state, sizeof (struct program));

  state_unhealthy_return2 (state, NULL);

  return program;
}


void
program_destroy (struct state *state, struct program *program)
{
  da_clear (state, &program->codes);
  da_clear (state, &program->constants);
  da_clear (state, &program->names);

  state_free (state, program);
}


void
program_mark (struct program *program)
{
  for (usize i = 0; i < program->constants.len; ++i)
    value_mark (program->constants.data[i]);
}


usize
program_append_code (struct state *state, struct program *program, insn i, struct location location)
{
  usize len = program->codes.len;

  struct code code = { location, i };

  da_append (state, &program->codes, code);

  state_unhealthy_return2 (state, 0);

  // Track stack size
  program->stack_head += insn_get_stack_delta (i);

  if (program->stack_head > program->stack_size)
    program->stack_size = program->stack_head;

  // Track scope size
  switch (insn_get_kind (i))
    {
    case I_STORE_NAME:
    case I_STORE_NAME_DROP:
      program->scope_size++;
      break;
    default:
      break;
    }

  return len;
}


usize
program_append_constant (struct state *state, struct program *program, struct value *constant)
{
  usize len = program->constants.len;

  da_append (state, &program->constants, constant);

  state_unhealthy_return2 (state, 0);

  return len;
}


usize
program_append_name (struct state *state, struct program *program, char *name)
{
  usize len = program->names.len;

  da_append (state, &program->names, name);

  state_unhealthy_return2 (state, 0);

  return len;
}


usize
program_hole (struct state *state, struct program *program, usize size)
{
  da_reserve (state, &program->codes, program->codes.len + size);

  state_unhealthy_return2 (state, 0);

  usize len = program->codes.len;

  program->codes.len += size;

  return len;
}


void
program_patch (struct program *program, usize index, insn i, struct location location)
{
  program->codes.data[index].i = i;

  program->codes.data[index].location = location;
}


void
program_show (FILE *stream, struct program *program)
{
  fprintf (stream, "<program %p>", (void *)program);
}


void
program_debug_print (struct program *program, usize depth)
{
  fprintf (stderr, "{\n");

  usize previous_line = -1;

  for (usize i = 0; i < program->codes.len; ++i)
    {
      struct code code = program->codes.data[i];

      struct location location = code.location;

      usize line = location.line;

      if (previous_line != line && i != 0)
        fprintf (stderr, "\n");

      if (previous_line != line)
        {
          if (location.context)
            fprintf (stderr, "%*zu ", (int)depth + 4, line);
          else
            fprintf (stderr, "%*s ", (int)depth + 4, "-");
        }
      else
        fprintf (stderr, "%*s ", (int)depth + 4, "");

      insn_debug_print (code.i, program, depth - 2);

      fprintf (stderr, "\n");

      previous_line = line;
    }

  fprintf (stderr, "\n");
  fprintf (stderr, "%*s", (int)depth, "");
  fprintf (stderr, "stack required: %zu\n", program->stack_size);

  fprintf (stderr, "%*s", (int)depth + 4 - 6, "");

  // if (depth > 6)
  //   fprintf (stderr, " ");

  fprintf (stderr, "}");
}

