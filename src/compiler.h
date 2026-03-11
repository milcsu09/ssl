#ifndef COMPILER_H
#define COMPILER_H


struct program;
struct state;
struct tree;


struct program *
compiler_compile (struct state *state, struct tree *tree);


#endif // COMPILER_H

