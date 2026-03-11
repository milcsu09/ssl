#ifndef INSN_H
#define INSN_H


#include "common.h"


struct program;


enum insn_kind
{
  // Discard the top of the stack.
  // (a -- )
  I_DROP,

  // Duplicate the top of the stack.
  // (a -- a a)
  I_DUPLICATE,

  // Push constants[DATA].
  // ( -- a)
  I_LOAD_CONST,

  // Push scope[names[DATA]].
  // ( -- a)
  I_LOAD_NAME,

  // Peek a value and store into scope[names[DATA]].
  // (a -- a)
  I_STORE_NAME,

  // Pop a value and store into scope[names[DATA]].
  // (a -- )
  I_STORE_NAME_DROP,

  // Pop DATA values and push them as a list.
  // (a_0 .. a_n -- LIST), n = DATA
  I_MAKE_LIST,

  // Pop a program value and push it as a callable lambda.
  // (PROGRAM -- LAMBDA)
  I_MAKE_LAMBDA,

  // Pop a list and push its tail, then its head.
  // (LIST -- TAIL HEAD)
  I_UNCONS,

  // Pop a value and set truth_flag based on whether it is truthy.
  // (a -- )
  I_IS_TRUTHY,

  // Pop a value and set truth_flag based on whether it equals the current top.
  // (a b -- a)
  I_IS_EQUAL,

  // Peek a value and set truth_flag based on whether it is a cons cell.
  // (a -- a)
  I_IS_CONS,

  // Jump DATA instructions forward (relative to next instruction)
  // (--)
  I_JUMP,

  // Jump DATA instructions forward (relative to next instruction), if truth_flag == false
  // (--)
  I_JUMP_FALSE,

  // Pop a callable and it's argument; create a new frame, with the argument on the stack.
  // (CALLABLE ARGUMENT -- RETVAL)
  I_CALL,

  // Pop a callable and it's argument; reuse current frame, with the argument on the stack.
  // (CALLABLE ARGUMENT -- RETVAL)
  I_CALL_TAIL,

  // Pop a callable and two arguments. This can only be a native, so no new stack frame.
  // (CALLABLE ARGUMENT -- RETVAL)
  I_CALL_BINARY,

  // Pop the return value and return to the caller.
  // (RETVAL -- )
  I_RETURN,
};


// NOTE: No out-of-bounds check!
const char *
insn_kind_string (enum insn_kind kind);


typedef u32 insn_data;

// [KIND (8) | DATA (24)]
typedef u32 insn;


insn
insn1 (enum insn_kind kind);

insn
insn2 (enum insn_kind kind, insn_data data);

enum insn_kind
insn_get_kind (insn i);

insn_data
insn_get_data (insn i);

s32
insn_get_stack_delta (insn i);

void
insn_debug_print (insn i, struct program *program, usize depth);


#endif // INSN_H

