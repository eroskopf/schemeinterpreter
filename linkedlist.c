#include <stdio.h>
#include <stdlib.h>
#include "talloc.h"
#include "linkedlist.h"
#include "value.h"
#include <string.h>
#include <stdbool.h>

// Create a new NULL_TYPE value node.
Value *makeNull() {
    //return a pointer pointing to a null value
    Value *n = talloc(sizeof(Value));
    n->type = NULL_TYPE;
    return n;
}

// Create a new CONS_TYPE value node.
Value *cons(Value *newCar, Value *newCdr) {
    Value *con = talloc(sizeof(Value));
    //should be pointers pointing to the car and the cdr
    con->type = CONS_TYPE;
    con->c.car = newCar;
    con->c.cdr = newCdr;
    return con;
}

//helper function to print an individual value
void helpPrint(Value *val) {
    switch (val->type) {
        case INT_TYPE:
            printf("%i",val->i);
            break;
        case DOUBLE_TYPE:
            printf("%f",val->d);
            break;
        case STR_TYPE:
            printf("%s",val->s);
            break;
        case CONS_TYPE:
            helpPrint(val->c.car);
            helpPrint(val->c.cdr);
            break;
        case NULL_TYPE:
            break;
        case PTR_TYPE:
            break;
        case SYMBOL_TYPE:
            printf("%s",val->s);
            break;
        case OPEN_TYPE:
            printf("(");
            break;
        case CLOSE_TYPE:
            printf(")");
            break;
        case BOOL_TYPE:
            printf("%s", val->s);
            break;
        case CLOSURE_TYPE:
            printf("#<procedure> \n");
            break;
        case VOID_TYPE:
            break;
        default: {
            texit(1);
        }
        // case OPENBRACKET_TYPE:
        //     printf("[");
        //     break;
        // case CLOSEBRACKET_TYPE:
        //     printf("]");
        //     break;
        // case SINGLEQUOTE_TYPE:
        //     printf("\'");
        //     break;
    }

}

// Display the contents of the linked list to the screen in some kind of
// readable format
void display(Value *list) {
    Value *cur = list;
    while (cur->type != NULL_TYPE) {
        printf("(");
        helpPrint(cur->c.car);
        printf(") \n");
        cur = cur->c.cdr;
    }
}


// Return a new list that is the reverse of the one that is passed in. All
// content within the list should be duplicated; all of the data should be shared
//
//
// FAQ: What if there are nested lists inside that list?
// ANS: There won't be for this assignment. There will be later, but that will
// be after we've got an easier way of managing memory.
Value *reverse(Value *list) {
    int length = 0;
    Value *new_head = makeNull();
    if (list->type == NULL_TYPE) {
        return new_head;
    }

    Value *curlen = list;
    while (curlen->type != NULL_TYPE) {
        curlen = cdr(curlen);
        length++;
    }

    //runs through the length
    for (int i = 0; i < length; i++) {
        Value *next = new_head;
        Value *cur = list;
        //inner for loop iterates until it finds the correct value we want to add
        //not very efficient but it works
        for (int j = 0; j < i; j++) {
            cur = cdr(cur);
        }
        //creates a new cons type, inserts the value and reattaches the old head
        new_head = talloc(sizeof(Value));
        new_head->type = CONS_TYPE;
        new_head->c.car = car(cur);
        new_head->c.cdr = next;
    }

    return new_head;
}


// Utility to make it less typing to get car value. Use assertions to make sure
// that this is a legitimate operation.
Value *car(Value *list) {
    return list->c.car;
}

// Utility to make it less typing to get cdr value. Use assertions to make sure
// that this is a legitimate operation.
Value *cdr(Value *list) {
    return list->c.cdr;
}

// Utility to check if pointing to a NULL_TYPE value. Use assertions to make sure
// that this is a legitimate operation.
bool isNull(Value *value) {
    if (value->type == NULL_TYPE) {
        return true;
    }
    return false;
}

// Measure length of list. Use assertions to make sure that this is a legitimate
// operation.
int length(Value *value) {
    Value *cur = value;
    int count = 0;

    while (cur->type != NULL_TYPE) {
        count++;
        cur = cur->c.cdr;
    }
    return count;
}