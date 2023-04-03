#include <stdlib.h>
#include "value.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>



//global variable linked list
Value *memList = NULL;

// Replacement for malloc that stores the pointers allocated. It should store
// the pointers in some kind of list; a linked list would do fine, but insert
// here whatever code you'll need to do so; don't call functions in the
// pre-existing linkedlist.h. Otherwise you'll end up with circular
// dependencies, since you're going to modify the linked list to use talloc.
void *talloc(size_t size) {
    Value *n = malloc(sizeof(Value) * size);
    Value *con = malloc(sizeof(Value));
    con->type = CONS_TYPE;
    con->c.car = n;
    con->c.cdr = memList;
    memList = con;
    return n;
}

// Free all pointers allocated by talloc, as well as whatever memory you
// allocated in lists to hold those pointers.
void tfree() {
    if (memList == NULL) return;

    while (memList != NULL) {
        Value *del = memList;
        memList = del->c.cdr;
        free(del->c.car);
        free(del);
    }
    free(memList);
}

// Replacement for the C function "exit", that consists of two lines: it calls
// tfree before calling exit. It's useful to have later on; if an error happens,
// you can exit your program, and all memory is automatically cleaned up.
void texit(int status) {
    tfree();
    exit(status);
}
