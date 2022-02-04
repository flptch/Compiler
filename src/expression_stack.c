// syntax_LL_stack
// xkrist24

#include <stdio.h>
#include <stdlib.h>
#include "expression_stack.h"
#define debugg  tToken tokenik;\
                tokenik.type = stack_top_type(&stack);\
                printf("__deb:___\ntop_token_type: %d\n\n", tokenik.type);\


//stack initialization
void stack_init(t_stack *stack){
    stack->item.type = tT_NULL;
    stack->next = NULL;
}


//returns first (top) item, returns NULL if empty
tToken stack_top(t_stack *stack){
    return stack->item;
}

//returns second item
tToken stack_sec(t_stack *stack){
    return stack->next->item;
}


//returns top if top is term else returns second item (2 nonterms forbidden)
tToken stack_top_term(t_stack *stack){
    if(stack->is_nonterm == false)
        return stack->item;
    return stack->next->item;
};


//returns type of first (top) item
tokenType stack_top_type(t_stack *stack){
    return stack->item.type;
}


bool stack_top_is_nonterm(t_stack *stack){
    return stack->is_nonterm;
}


//deletes first element  in stack, second goes to top
void stack_pop(t_stack *stack){
    if(!stack)
        return;
    t_stack *to_delete;
    to_delete = stack->next;
    stack->item = stack->next->item;
    stack->is_nonterm = stack->next->is_nonterm;    
    stack->next = stack->next->next;
    free(to_delete);
}


//inserts new element on top
void stack_push_term(t_stack *stack, tToken token){
    t_stack *new = (t_stack*) malloc(sizeof(t_stack)); 
    new->item = stack->item;        
    new->next = stack->next;
    new->is_nonterm = stack->is_nonterm;
    stack->next = new;         
    stack->item = token;
    stack->is_nonterm = false;
}

//inserts new element on top
void stack_push_nonterm(t_stack *stack, tToken token){
    t_stack *new = (t_stack*) malloc(sizeof(t_stack)); 
    new->item = stack->item;        
    new->next = stack->next;
    new->is_nonterm = stack->is_nonterm;
    stack->next = new;         
    stack->item = token;
    stack->is_nonterm = true;
}


void stack_popfrom_pushto(t_stack *stackpop, t_stack *stackpush){    
    if(stack_top_is_nonterm(stackpop) == true)
        stack_push_nonterm(stackpush, stackpop->item);
    else
        stack_push_term(stackpush, stackpop->item);
    stack_pop(stackpop);
}


//frees whole stack
void stack_dispose(t_stack *stack){
    stack = stack->next;                //first element is not allocated
    while(stack){
        t_stack *next = stack->next;
        free(stack); 
        stack = next;
    }
}


//stack print for debugging purposes
void stack_print(t_stack *stack){
    while(stack->next){
        if(stack_top_is_nonterm(stack))
            printf("stack type: %d\t[E]\n", stack->item.type);
        else
            printf("stack type: %d\n", stack->item.type);
        stack = stack->next;
    }
}

#define DOLLAR 200
#define STOP 202
void stack_visualize(t_stack *stack){
    char str;
    if(stack->is_nonterm)
        str = 'E';
    else
        switch(stack->item.type){
            case tT_PLUS: str = '+'; break;
            case tT_MINUS: str = '-'; break;
            case tT_MUL: str = '*'; break;
            case tT_DIV: str = '/'; break;
            case tT_IDIV: str = '/'; break;
            //case DOLLAR: str = '$'; break;
            //case STOP: str = '<'; break;
            case tT_INT: str = 'i'; break;
            case tT_LBRACKET: str = '('; break;
            //case tT_PLUS: str = '+'; break;
            default: str = '?'; return;
        }
    stack_visualize(stack->next);
    printf("%c", str);
}