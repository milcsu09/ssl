#ifndef EVALUATOR_H
#define EVALUATOR_H

struct ast;
struct table;
struct value;

struct value *evaluate (struct ast *, struct table *);

#endif // EVALUATOR_H

