#include "compiler.h"
#include "program.h"
#include "state.h"
#include "string.h"
#include "tree.h"
#include "value.h"


void
compiler_compile_program (struct state *state, struct tree *tree, struct program *program,
                          bool tail);


struct fail_hole
{
  usize hole;
  usize depth;
};


struct fail_hole_da
{
  struct fail_hole *data;
  usize len;
  usize cap;
};


static void
fail_hole_append (struct state *state, struct fail_hole_da *da, usize hole, usize depth)
{
  struct fail_hole fail_hole;

  fail_hole.hole = hole;
  fail_hole.depth = depth;

  da_append (state, da, fail_hole);
}


void
compiler_compile_pattern (struct state *state, struct tree *tree, struct program *program,
                          struct fail_hole_da *fails, usize *depth)
{
  switch (tree->kind)
    {
    case TREE_PATTERN_WILDCARD:
      {
        program_append_code (state, program, insn1 (I_DROP), tree->location);

        state_unhealthy_return1 (state);

        (*depth)--;
      }
      break;

    case TREE_PATTERN_NAME:
      {
        struct tree *node_head = tree->child;

        usize name = program_append_name (state, program, token_unbox_s (node_head->token));

        state_unhealthy_return1 (state);

        program_append_code (state, program, insn2 (I_STORE_NAME_DROP, name), node_head->location);

        state_unhealthy_return1 (state);

        (*depth)--;
      }
      break;

    case TREE_PATTERN_LITERAL:
      {
        compiler_compile_program (state, tree->child, program, false);

        state_unhealthy_return1 (state);

        (*depth)++;

        program_append_code (state, program, insn1 (I_IS_EQUAL), location_none);

        state_unhealthy_return1 (state);

        (*depth)--;

        usize hole = program_hole (state, program, 1);

        state_unhealthy_return1 (state);

        fail_hole_append (state, fails, hole, *depth - 1);

        state_unhealthy_return1 (state);

        program_append_code (state, program, insn1 (I_DROP), tree->location);

        state_unhealthy_return1 (state);

        (*depth)--;
      }
      break;

    case TREE_PATTERN_LIST_EMPTY:
      {
        struct value *c = value_box_li (state, NULL);

        state_unhealthy_return1 (state);

        usize ci = program_append_constant (state, program, c);

        state_unhealthy_return1 (state);

        program_append_code (state, program, insn2 (I_LOAD_CONST, ci), tree->location);

        state_unhealthy_return1 (state);

        (*depth)++;

        program_append_code (state, program, insn1 (I_IS_EQUAL), location_none);

        state_unhealthy_return1 (state);

        (*depth)--;

        usize hole = program_hole (state, program, 1);

        state_unhealthy_return1 (state);

        fail_hole_append (state, fails, hole, *depth - 1);

        state_unhealthy_return1 (state);

        program_append_code (state, program, insn1 (I_DROP), tree->location);

        state_unhealthy_return1 (state);

        (*depth)--;
      }
      break;

    case TREE_PATTERN_LIST_CONS:
      {
        struct tree *node_head = tree->child;
        struct tree *node_tail = tree->child->next;

        program_append_code (state, program, insn1 (I_IS_CONS), location_none);

        state_unhealthy_return1 (state);

        usize hole = program_hole (state, program, 1);

        state_unhealthy_return1 (state);

        fail_hole_append (state, fails, hole, *depth - 1);

        state_unhealthy_return1 (state);

        program_append_code (state, program, insn1 (I_UNCONS), tree->location);

        state_unhealthy_return1 (state);

        (*depth)++;

        compiler_compile_pattern (state, node_head, program, fails, depth);

        state_unhealthy_return1 (state);

        compiler_compile_pattern (state, node_tail, program, fails, depth);

        state_unhealthy_return1 (state);
      }

    default:
      break;
    }
}


void
compiler_compile_program (struct state *state, struct tree *tree, struct program *program,
                          bool tail)
{
  switch (tree->kind)
    {
    case TREE_PROGRAM:
      for (struct tree *p = tree->child; p; p = p->next)
        {
          bool is_last = p->next == NULL;

          compiler_compile_program (state, p, program, tail && is_last);

          state_unhealthy_return1 (state);

          if (!is_last)
            {
              program_append_code (state, program, insn1 (I_DROP), location_none);

              state_unhealthy_return1 (state);
            }
        }
      break;

    case TREE_ASSIGNMENT:
      {
        struct tree *node_head = tree->child;
        struct tree *node_body = tree->child->next;

        char *s = token_unbox_s (node_head->token);

        compiler_compile_program (state, node_body, program, false);

        state_unhealthy_return1 (state);

        usize name = program_append_name (state, program, s);

        state_unhealthy_return1 (state);

        program_append_code (state, program, insn2 (I_STORE_NAME, name), node_head->location);

        state_unhealthy_return1 (state);
      }
      break;

    case TREE_LAMBDA:
      {
        struct tree *node_head = tree->child;
        struct tree *node_body = tree->child->next;

        char *s = token_unbox_s (node_head->token);

        struct program *inner = program_create (state);

        state_unhealthy_return1 (state);

        inner->stack_head = 1;
        inner->stack_size = 1;

        usize ai = program_append_name (state, inner, s);

        state_unhealthy_return1 (state);

        // STORE_NAME (argument)
        program_append_code (state, inner, insn2 (I_STORE_NAME_DROP, ai), location_none);

        state_unhealthy_return1 (state);

        // BODY
        compiler_compile_program (state, node_body, inner, true);

        state_unhealthy_return1 (state);

        // RETURN
        program_append_code (state, inner, insn1 (I_RETURN), location_none);

        state_unhealthy_return1 (state);

        struct value *c = value_box_p (state, inner);

        state_unhealthy_return1 (state);

        usize ci = program_append_constant (state, program, c);

        state_unhealthy_return1 (state);

        program_append_code (state, program, insn2 (I_LOAD_CONST, ci), node_head->location);

        state_unhealthy_return1 (state);

        program_append_code (state, program, insn1 (I_MAKE_LAMBDA), node_head->location);

        state_unhealthy_return1 (state);
      }
      break;

    case TREE_MATCH:
      {
        struct tree *node_head = tree->child;
        struct tree *node_body = tree->child->next;

        compiler_compile_program (state, node_head, program, tail);

        state_unhealthy_return1 (state);

        compiler_compile_program (state, node_body, program, tail);

        state_unhealthy_return1 (state);
      }
      break;

    case TREE_MATCH_ARM:
      {
        struct tree *node_head = tree->child;
        struct tree *node_body = tree->child->next;
        struct tree *node_rest = tree->child->next->next;

        program_append_code (state, program, insn1 (I_DUPLICATE), location_none);

        state_unhealthy_return1 (state);

        // Save stack before body
        usize saved_head = program->stack_head;

        struct fail_hole_da fails = {0};

        usize stack_head = 1;

        compiler_compile_pattern (state, node_head, program, &fails, &stack_head);

        if (state->unhealthy)
          {
            da_clear (state, &fails);
            return;
          }

        program_append_code (state, program, insn1 (I_DROP), location_none);

        if (state->unhealthy)
          {
            da_clear (state, &fails);
            return;
          }

        compiler_compile_program (state, node_body, program, tail);

        if (state->unhealthy)
          {
            da_clear (state, &fails);
            return;
          }

        usize hole_end = program_hole (state, program, 1);

        if (state->unhealthy)
          {
            da_clear (state, &fails);
            return;
          }

        // Restore stack after hole_end (JUMP)
        program->stack_head = saved_head;

        usize max_drops = 0;

        for (usize i = 0; i < fails.len; i++)
          {
            usize drops = fails.data[i].depth + 1;
            if (drops > max_drops)
              max_drops = drops;
          }

        usize drop_start = program->codes.len;

        for (usize j = 0; j < max_drops; j++)
          {
            program_append_code (state, program, insn1 (I_DROP), location_none);

            if (state->unhealthy)
              {
                da_clear (state, &fails);
                return;
              }
          }

        for (usize i = 0; i < fails.len; i++)
          {
            usize h      = fails.data[i].hole;
            usize drops  = fails.data[i].depth + 1;
            usize target = drop_start + (max_drops - drops);
            usize distance = target - h - 1;

            program_patch (program, h, insn2 (I_JUMP_FALSE, distance), location_none);
          }

        da_clear (state, &fails);

        if (node_rest)
          {
            compiler_compile_program (state, node_rest, program, tail);

            state_unhealthy_return1 (state);
          }
        else
          {
            program_append_code (state, program, insn1 (I_DROP), location_none);

            state_unhealthy_return1 (state);

            struct value *c = value_box_u (state);

            state_unhealthy_return1 (state);

            usize ci = program_append_constant (state, program, c);

            state_unhealthy_return1 (state);

            program_append_code (state, program, insn2 (I_LOAD_CONST, ci), location_none);

            state_unhealthy_return1 (state);
          }

        usize distance = program->codes.len - hole_end - 1;

        program_patch (program, hole_end, insn2 (I_JUMP, distance), location_none);
      }
      break;

    case TREE_BRANCH:
      {
        struct tree *node_condition = tree->child;
        struct tree *node_then = tree->child->next;
        struct tree *node_else = tree->child->next->next;

        usize distance;

        compiler_compile_program (state, node_condition, program, false);

        state_unhealthy_return1 (state);

        program_append_code (state, program, insn1 (I_IS_TRUTHY), location_none);

        state_unhealthy_return1 (state);

        usize hole_then = program_hole (state, program, 1);

        state_unhealthy_return1 (state);

        // Save stack before body
        usize saved_head = program->stack_head;

        compiler_compile_program (state, node_then, program, tail);

        state_unhealthy_return1 (state);

        usize hole_else = program_hole (state, program, 1);

        state_unhealthy_return1 (state);

        distance = program->codes.len - hole_then - 1;

        program_patch (program, hole_then, insn2 (I_JUMP_FALSE, distance), location_none);

        // Save stack after hole_end (JUMP)
        program->stack_head = saved_head;

        if (node_else)
          {
            compiler_compile_program (state, node_else, program, tail);

            state_unhealthy_return1 (state);
          }
        else
          {
            struct value *c = value_box_u (state);

            state_unhealthy_return1 (state);

            usize ci = program_append_constant (state, program, c);

            state_unhealthy_return1 (state);

            program_append_code (state, program, insn2 (I_LOAD_CONST, ci), location_none);

            state_unhealthy_return1 (state);
          }

        distance = program->codes.len - hole_else - 1;

        program_patch (program, hole_else, insn2 (I_JUMP, distance), location_none);
      }
      break;

    case TREE_CALL:
      {
        struct tree *node_f = tree->child;
        struct tree *node_x = tree->child->next;

        compiler_compile_program (state, node_f, program, false);

        state_unhealthy_return1 (state);

        compiler_compile_program (state, node_x, program, false);

        state_unhealthy_return1 (state);

        if (tail)
          program_append_code (state, program, insn1 (I_CALL_TAIL), node_f->location);
        else
          program_append_code (state, program, insn1 (I_CALL), node_f->location);

        state_unhealthy_return1 (state);
      }
      break;

    case TREE_CALL_BINARY:
      {
        struct tree *node_f = tree->child;
        struct tree *node_a = tree->child->next;
        struct tree *node_b = tree->child->next->next;

        compiler_compile_program (state, node_f, program, false);

        state_unhealthy_return1 (state);

        compiler_compile_program (state, node_a, program, false);

        state_unhealthy_return1 (state);

        compiler_compile_program (state, node_b, program, false);

        state_unhealthy_return1 (state);

        program_append_code (state, program, insn1 (I_CALL_BINARY), node_f->location);

        state_unhealthy_return1 (state);
      }
      break;

    case TREE_UNIT:
      {
        struct value *c = value_box_u (state);

        state_unhealthy_return1 (state);

        usize ci = program_append_constant (state, program, c);

        state_unhealthy_return1 (state);

        program_append_code (state, program, insn2 (I_LOAD_CONST, ci), tree->location);

        state_unhealthy_return1 (state);
      }
      break;

    case TREE_LIST:
      {
        usize n = 0;

        for (struct tree *p = tree->child; p; p = p->next)
          {
            compiler_compile_program (state, p, program, false);

            state_unhealthy_return1 (state);

            n++;
          }

        if (n == 0)
          {
            struct value *c = value_box_li (state, NULL);

            state_unhealthy_return1 (state);

            usize ci = program_append_constant (state, program, c);

            state_unhealthy_return1 (state);

            program_append_code (state, program, insn2 (I_LOAD_CONST, ci), tree->location);

            state_unhealthy_return1 (state);
          }
        else
          {
            program_append_code (state, program, insn2 (I_MAKE_LIST, n), tree->location);

            state_unhealthy_return1 (state);
          }
      }
      break;

    case TREE_INTEGER:
      {
        struct value *c = value_box_i (state, token_unbox_i (tree->token));

        state_unhealthy_return1 (state);

        usize ci = program_append_constant (state, program, c);

        state_unhealthy_return1 (state);

        program_append_code (state, program, insn2 (I_LOAD_CONST, ci), tree->location);

        state_unhealthy_return1 (state);
      }
      break;

    case TREE_FLOAT:
      {
        struct value *c = value_box_f (state, token_unbox_f (tree->token));

        state_unhealthy_return1 (state);

        usize ci = program_append_constant (state, program, c);

        state_unhealthy_return1 (state);

        program_append_code (state, program, insn2 (I_LOAD_CONST, ci), tree->location);

        state_unhealthy_return1 (state);
      }
      break;

    case TREE_STRING:
      {
        char *s = c_string_copy (state, token_unbox_s (tree->token), false);

        state_unhealthy_return1 (state);

        struct value *c = value_box_s (state, s);

        state_unhealthy_return1 (state);

        usize ci = program_append_constant (state, program, c);

        state_unhealthy_return1 (state);

        program_append_code (state, program, insn2 (I_LOAD_CONST, ci), tree->location);

        state_unhealthy_return1 (state);
      }
      break;

    case TREE_NAME:
      {
        char *s = tree->token->data.s;

        usize name = program_append_name (state, program, s);

        state_unhealthy_return1 (state);

        program_append_code (state, program, insn2 (I_LOAD_NAME, name), tree->location);

        state_unhealthy_return1 (state);
      }
      break;

    default:
      break;
    }
}


struct program *
compiler_compile (struct state *state, struct tree *tree)
{
  struct program *program;

  program = program_create (state);

  state_unhealthy_return2 (state, NULL);

  compiler_compile_program (state, tree, program, false);

  state_unhealthy_return2 (state, NULL);

  return program;
}

