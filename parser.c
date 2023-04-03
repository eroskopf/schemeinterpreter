#include <stdio.h>
#include <stdlib.h>
#include "talloc.h"
#include "linkedlist.h"
#include "value.h"
#include "parser.h"
#include <string.h>
#include <stdbool.h>
#include "value.h"
#include "talloc.h"

//add to parse tree helper function
Value *addToParseTree(Value *tree, int *depth, Value *token) {
    Value *curr = tree;
    Value *list = makeNull();
    if (token->type == CLOSE_TYPE) {
        depth--;
        if (depth < 0) {
            printf("Syntax error: too many close parentheses\n");
            texit(1);
        }
        //close parenthesis so have to pop everything out
        while (car(curr)->type != OPEN_TYPE) {
            list = cons(car(curr), list);
            curr = cdr(curr);
        }
        //curr is now an open parenthesis
        curr = cdr(curr);
        tree = cons(list, curr);

    } else {
        //push onto the list
        if (token->type == OPEN_TYPE) {
            depth++;
        }
        tree = cons(token, tree);
    }
    return tree;
}

// Takes a list of tokens from a Scheme program, and returns a pointer to a
// parse tree representing that program.
Value *parse(Value *tokens) {

    Value *tree = makeNull();
    int depth = 0;

    Value *current = tokens;
    //assert(current != NULL && "Error (parse): null pointer");
    while (current->type != NULL_TYPE) {
        Value *token = car(current);
        tree = addToParseTree(tree, &depth, token);
        current = cdr(current);
    }
    if (depth != 0) {
        printf("Syntax error: not enough close parentheses\n");
        texit(1);
    }
    Value *rev = reverse(tree);
    return rev;
}

// Prints the tree to the screen in a readable fashion. It should look just like
// Scheme code; use parentheses to indicate subtrees.
// Prints the tree to the screen in a readable fashion. It should look just like
// Racket code; use parentheses to indicate subtrees.
void printTree(Value *tree) {
    Value *curr = tree;
    while (curr->type != NULL_TYPE) {
        if (car(curr)->type == CONS_TYPE) {
            //printing a nested tree
            printf("(");
            printTree(car(curr));
            printf(") ");
        }
        else if (car(curr)->type == NULL_TYPE) {
            //null type printing is ()
            printf("()");
        }
        else {
            //printing everything else, without the () around it
            Value *carr = car(curr);
            int car_type = carr->type;
            //doesn't like me not dealing with cons types and 
            //this weird int type this is a workaround
            switch (car_type) {
                case BOOL_TYPE:
                    printf("%s ", carr->s);
                    break;
                case INT_TYPE:
                    printf("%i ", carr->i);
                    break;
                case DOUBLE_TYPE:
                    printf("%f ", carr->d);
                    break;
                case STR_TYPE:
                    printf("\"%s\" ", carr->s);
                    break;
                case SYMBOL_TYPE:
                    printf("%s ", carr->s);
                    break;
            }
        }
        curr = cdr(curr);
    }
}
