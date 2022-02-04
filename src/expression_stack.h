// expression_stack.h
// xkrist24

#ifndef expression_stack
#define expression_stack

#include <stdbool.h>
#include "scanner.h"
#define tT_NULL 100

//typedef struct stack_item t_stack;

typedef struct t_stack{
    tToken item;
    bool is_nonterm;    
    struct t_stack *next;
}t_stack;

void stack_init(t_stack *stack);
tToken stack_top(t_stack *stack);
tToken stack_sec(t_stack *stack);
tToken stack_top_term(t_stack *stack);
tokenType stack_top_type(t_stack *stack);
bool stack_top_is_nonterm(t_stack *stack);
void stack_pop(t_stack *stack);
void stack_push_term(t_stack *stack, tToken token);
void stack_push_nonterm(t_stack *stack, tToken token);
void stack_popfrom_pushto(t_stack *stackpop, t_stack *stackpush);
void stack_dispose(t_stack *stack);

void stack_print(t_stack *stack);
void stack_visualize(t_stack *stack);

#endif //expression_stack