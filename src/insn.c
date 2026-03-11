#include "insn.h"
#include "program.h"
#include "value.h"

#include <stdio.h>



static const char *const INSN_KIND_STRING[] = {
  "DROP",
  "DUPLICATE",
  "LOAD_CONST",
  "LOAD_NAME",
  "STORE_NAME",
  "STORE_NAME_DROP",
  "MAKE_LIST",
  "MAKE_LAMBDA",
  "UNCONS",
  "IS_TRUTHY?",
  "IS_EQUAL?",
  "IS_CONS?",
  "JUMP",
  "JUMP_FALSE",
  "CALL",
  "CALL_TAIL",
  "CALL_BINARY",
  "RETURN",
};


const char *
insn_kind_string (enum insn_kind kind)
{
  return INSN_KIND_STRING[kind];
}


insn
insn1 (enum insn_kind kind)
{
  return (insn)kind << 24;
}


insn
insn2 (enum insn_kind kind, insn_data data)
{
  return ((insn)kind << 24) | (data & 0xFFFFFF);
}


enum insn_kind
insn_get_kind (insn i)
{
  return i >> 24;
}


insn_data
insn_get_data (insn i)
{
  return i & 0xFFFFFF;
}


s32
insn_get_stack_delta (insn i)
{
  insn_data d = insn_get_data (i);

  switch (insn_get_kind (i))
    {
    case I_DROP:
      return -1;
    case I_DUPLICATE:
      return +1;
    case I_LOAD_CONST:
      return +1;
    case I_LOAD_NAME:
      return +1;
    case I_STORE_NAME:
      return -1 + 1;
    case I_STORE_NAME_DROP:
      return -1;
    case I_MAKE_LIST:
      return -d + 1;
    case I_MAKE_LAMBDA:
      return -1 + 1;
    case I_UNCONS:
      return -1 + 2;
    case I_IS_TRUTHY:
      return -1;
    case I_IS_EQUAL:
      return -1;
    case I_IS_CONS:
      return 0;
    case I_JUMP:
      return 0;
    case I_JUMP_FALSE:
      return 0;
    case I_CALL:
      return -2 + 1;
    case I_CALL_TAIL:
      return -2 + 1;
    // case I_CALL_BINARY:
    //   return -3 + 1;
    case I_RETURN:
      return -1;
    default:
      return 0;
    }
}


void
insn_debug_print (insn i, struct program *program, usize depth)
{
  enum insn_kind k = insn_get_kind (i);

  insn_data d = insn_get_data (i);

  switch (k)
    {
    case I_IS_TRUTHY:
    case I_IS_EQUAL:
    case I_IS_CONS:
      fprintf (stderr, "\033[94m");
      break;
    case I_CALL:
    case I_CALL_TAIL:
    case I_CALL_BINARY:
    case I_RETURN:
      fprintf (stderr, "\033[91m");
      break;
    default:
      fprintf (stderr, "\033[96m");
      break;
    }

  fprintf (stderr, "%-15s\033[0m", insn_kind_string (k));

  switch (k)
    {
    case I_LOAD_CONST:
      fprintf (stderr, " %4d (", d);

      if (program->constants.data[d]->kind == VALUE_PROGRAM)
        program_debug_print (program->constants.data[d]->data.p, depth + 26);
      else
        {
          fprintf (stderr, "\033[92m");
          value_show (stderr, program->constants.data[d]);
          fprintf (stderr, "\033[0m");
        }

      fprintf (stderr, ")");
      break;
    case I_LOAD_NAME:
      fprintf (stderr, " %4d (\033[92m%s\033[0m)", d, program->names.data[d]);
      break;
    case I_STORE_NAME:
    case I_STORE_NAME_DROP:
      fprintf (stderr, " %4d (\033[92m%s\033[0m)", d, program->names.data[d]);
      break;
    case I_MAKE_LIST:
      fprintf (stderr, " %4d", d);
      break;
    case I_JUMP:
      fprintf (stderr, " %+4d", d);
      break;
    case I_JUMP_FALSE:
      fprintf (stderr, " %+4d", d);
      break;
    default:
      break;
    }
}

