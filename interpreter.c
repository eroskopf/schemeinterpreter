#include <stdio.h>
#include <stdlib.h>
#include "talloc.h"
#include "linkedlist.h"
#include "value.h"
#include "parser.h"
#include <string.h>
#include <stdbool.h>
#include "interpreter.h"

void displayResults(Value *result) {
    Value *curr = result;
    int currtype = curr->type;
    switch (currtype)  {
    case VOID_TYPE: {
        break;
    }
    case NULL_TYPE: {
        printf("()");
        break;
    }
    case INT_TYPE: {
        printf("%i", curr->i);
        break;
        }
    case BOOL_TYPE:
        printf("%s", curr->s);
        break;
    case DOUBLE_TYPE:
        printf("%f", curr->d);
        break;
    case STR_TYPE:
        printf("\"%s\"", curr->s);
        break;
    case CONS_TYPE: {
        printf("(");
        while(curr->type == CONS_TYPE){
            displayResults(car(curr));
            curr = cdr(curr);
            if(curr->type != CONS_TYPE && curr->type != NULL_TYPE){
                printf(" . ");
                displayResults(curr);
            } else if(curr->type != NULL_TYPE ){
                printf(" ");
            }
        }
        printf(")");
        break;
     }
    case SYMBOL_TYPE:
        printf("%s", curr->s);
        break;
    case CLOSURE_TYPE:
        printf("#<procedure>");
        break;
    default:
        break;
    } 
}

Value *primitiveGreater(Value *args){
    Value *result = talloc(sizeof(Value));
    result->type = BOOL_TYPE;
    result->s = talloc(sizeof(char) * 3);
    Value *arg1 = car(args);
    Value *arg2 = car(cdr(args));

    if (arg1->type == arg2->type) {
        bool isGreater;
        switch (arg1->type) {
            case INT_TYPE:
                isGreater = arg1->i > arg2->i;
                break;
            case DOUBLE_TYPE:
                isGreater = arg1->d > arg2->d;
                break;
            default:
                printf("Evaluation error: greater than needs two arguments of the same type\n");
                texit(1);
        }
        if (isGreater) {
            memcpy(result->s, "#t", strlen("#t") + 1);
        } else {
            memcpy(result->s, "#f", strlen("#f") + 1);
            }
    } else {
        memcpy(result->s, "#f", strlen("#f") + 1);
    }
    return result;
}

//Greater but inverse
Value *primitiveLess(Value *args){
    Value *result = talloc(sizeof(Value));
    result->type = BOOL_TYPE;
    result->s = talloc(sizeof(char) * 3);
    Value *arg1 = car(args);
    Value *arg2 = car(cdr(args));

    if (arg1->type == arg2->type) {
    bool isLesser;
    switch (arg1->type) {
        case INT_TYPE:
            isLesser = arg1->i < arg2->i;
            break;
        case DOUBLE_TYPE:
            isLesser = arg1->d < arg2->d;
            break;
        default:
            printf("Evaluation error: lesser than needs two arguments of the same type\n");
            texit(1);
    }
    if (isLesser) {
        memcpy(result->s, "#t", strlen("#t") + 1);
    } else {
        memcpy(result->s, "#f", strlen("#f") + 1);
        }
    } else {
    memcpy(result->s, "#f", strlen("#f") + 1);
    }
    return result;
}

//basically the same as the above but changing the boolean condition
Value *primitiveEqual(Value *args){
    Value *result = talloc(sizeof(Value));
    result->type = BOOL_TYPE;
    result->s = talloc(sizeof(char) * 3);
    Value *arg1 = car(args);
    Value *arg2 = car(cdr(args));
    
    if (arg1->type == arg2->type) {
        bool isEqual;
        switch (arg1->type) {
            case INT_TYPE:
                isEqual = arg1->i == arg2->i;
                break;
            case DOUBLE_TYPE:
                isEqual = arg1->d == arg2->d;
            break;
        default:
            printf("Evaluation error: equal needs two arguments of the same type\n");
            texit(1);
    }

    if (isEqual) {
        memcpy(result->s, "#t", strlen("#t") + 1);
    } else {
        memcpy(result->s, "#f", strlen("#f") + 1);
    }
    } else {
    memcpy(result->s, "#f", strlen("#f") + 1);
}

return result;
}

//do same thing as + essentially
Value *primitiveMinus(Value *args){
    Value *node = talloc(sizeof(Value));
    node->type = INT_TYPE;
    Value* curr = args;
    int first = 1;
    int sum = 0;
    double sumD = 0;
    while(!isNull(curr) && !isNull(car(curr))){
        if(car(curr)->type != DOUBLE_TYPE && car(curr)->type != INT_TYPE){
            printf("Evaluation error: - requires int or double type arguments\n");
            texit(1);
        }
        if(car(curr)->type == DOUBLE_TYPE && node->type == INT_TYPE){
            node->type = DOUBLE_TYPE;
            sumD = sum;
        }
        if(node->type == DOUBLE_TYPE){
            if(first == 1){
                sumD = car(curr)->d;
                first = 0;
            } else{
                if(car(curr)->type == DOUBLE_TYPE){
                    sumD -= car(curr)->d;
                } else{
                    sumD -= car(curr)->i;
                }   
            }
        }else{
            if(first == 1){
                sum = car(curr)->i;
                first = 0;
            } else{
                if(car(curr)->type == DOUBLE_TYPE){
                    sum -= car(curr)->d;
                } else{
                    sum -= car(curr)->i;
                } 
            }
        }
        curr = cdr(curr);
    }
    if(node->type == DOUBLE_TYPE){
        node->d = sumD;
    } else{
        node->i = sum;
    }
    return node;
}

Value *primitiveDivide(Value *args){
    Value *node = talloc(sizeof(Value));
    //only takes in two values, don't need to check
    Value *numOne = car(args);
    Value *numTwo = car(cdr(args));
    //type checking to make sure the correct i or d is called
    if(numOne->type == INT_TYPE){
        if(numTwo->type == INT_TYPE){
            if(numOne->i % numTwo->i == 0){
                node->type = INT_TYPE;
                node->i = numOne->i/numTwo->i;
            } else{
                node->type = DOUBLE_TYPE;
                node->d = numOne->i/(float)numTwo->i;
            }
        } else{
            node->type = DOUBLE_TYPE;
            node->d = numOne->i/numTwo->d; 
        }
    } else{
        node->type = DOUBLE_TYPE;
        if(numTwo->type == DOUBLE_TYPE){
            node->d = numOne->d/numTwo->d;
        } else{
            node->d = numOne->d/numTwo->i;
        }
        
    }
    return node;   
}

Value *primitiveModulo (Value *args) {
    //only takes in 2 arguments, both ints
    Value *node = talloc(sizeof(Value));
    node->type = INT_TYPE;
    node->i = (car(args)->i) % (car(cdr(args))->i);
    return node;
}

Value *primitiveMult(Value *args){
    Value *node = talloc(sizeof(Value));
    node->type = INT_TYPE;
    Value* curr = args;
    int sum = 1;
    double sumD = 1;
    //has to be able to accept lots of values
    while(!isNull(curr) && !isNull(car(curr))){
        if(car(curr)->type != DOUBLE_TYPE && car(curr)->type != INT_TYPE){
            printf("Evaluation error: * requires arguments that are ints or doubles\n");
            texit(1);
        }
        else if(car(curr)->type == DOUBLE_TYPE && node->type == INT_TYPE){
            node->type = DOUBLE_TYPE;
            sumD = sum;
        }
        else if(node->type == DOUBLE_TYPE){
            if(car(curr)->type == DOUBLE_TYPE){
                sumD *= car(curr)->d;
            } else{
                sumD *= car(curr)->i;
            }   

        }else{
            if(car(curr)->type == DOUBLE_TYPE){
                sum *= car(curr)->d;
            } else{
                sum *= car(curr)->i;
            } 
        }
        curr = cdr(curr);
    }
    if(node->type == DOUBLE_TYPE){
        node->d = sumD;
    } else{
        node->i = sum;
    }
    return node;
}

Value *primitiveCar(Value *args) {
    if(isNull(args) || isNull(car(args)) || car(args)->type != CONS_TYPE || !isNull(cdr(args))){
        printf("Evaluation error: car has wrong number or type of arguments\n");
        texit(1);
    }
    Value *result = talloc(sizeof(Value));
    result->type = CONS_TYPE;
    if (args->type == CONS_TYPE && car(args)->type != NULL_TYPE && cdr(args)->type == NULL_TYPE) {
        result = car(car(args));
    }
    return result;
}

Value *primitiveCdr(Value *args) {
    if(isNull(args) || isNull(car(args)) || car(args)->type != CONS_TYPE || !isNull(cdr(args))){
        printf("Evaluation error: cdr has wrong number or type of arguments\n");
        texit(1);
    }
    Value *result = talloc(sizeof(Value));
    result->type = CONS_TYPE;
    if (args->type == CONS_TYPE && car(args)->type != NULL_TYPE && cdr(args)->type == NULL_TYPE) {
        result = cdr(car(args));
    }
    return result;
}

Value *primitiveNull(Value *args){
    if(isNull(args)){
        printf("Evaluation error: no inputs \n");
        texit(1);
    }
    if(!isNull(cdr(args))){
        printf("Evaluation error: null? only takes 1 input\n");
        texit(1);
    }
    Value *bool_val = talloc(sizeof(Value));
    bool_val->type = BOOL_TYPE;
    bool_val->s = talloc(sizeof(char) * 3);
    if (isNull(car(args)) || (car(args)->type == CONS_TYPE && (car(car(args)))->type == NULL_TYPE)){
        memcpy(bool_val->s, "#t", strlen("#t")+1);
    } else{
        memcpy(bool_val->s, "#f", strlen("#f")+1);
    }
    return bool_val;
}

Value *primitiveCons(Value *args) {
    Value *result = talloc(sizeof(Value));
    result->type = CONS_TYPE;
    if (length(args)!=2) {
        //cons can take in null
        //args->c.car->type == NULL_TYPE || args->c.car->c.cdr->type == NULL_TYPE 
        printf("Evaluation error: cons takes in two inputs\n");
        texit(1);
   }
   //could be int, double, string, bool, symbol, cons, null, quoted inputs, pretty much anything
   //the dot type situation is handled in display not here
   result->c.car = car(args);
   result->c.cdr = car(cdr(args));
   return result;
}

Value *primitiveAdd(Value *args) {
    Value *result = talloc(sizeof(Value));
    result->type = INT_TYPE;
    double result_num = 0;    
    Value *cur_arg = args; 
    while (cur_arg->type != NULL_TYPE) {
        Value *cur_num = car(cur_arg);
        if (cur_num->type != INT_TYPE) {
            if (cur_num->type == DOUBLE_TYPE) {
                result->type = DOUBLE_TYPE;
                result_num += cur_num->d;
            } else {
                printf("Evaluation error: wrong type for add\n");
                texit(1);
            }
        } else {
            result_num += cur_num->i;    
        }
        cur_arg = cdr(cur_arg);
    }
    
    if (result->type == INT_TYPE) {
        result->i = (int) result_num;
    } else {
        result->d = result_num;
    }
    return result;
}

void bind(char *name, Value *(*function)(struct Value *), Frame *frame) {
    // Add primitive functions to top-level bindings list
    Value *value = talloc(sizeof(Value));
    value->type = PRIMITIVE_TYPE;
    value->pf = function;
    //need to have the name of the function
    Value *n = talloc(sizeof(Value));
    n->type = SYMBOL_TYPE;
    int len = strlen(name) + 1;
    n->s = talloc(len*sizeof(char));
    memcpy(n->s, name, strlen(name)+1); 
    strcpy(n->s, name);
    frame->bindings = cons(cons(n, value), frame->bindings);
}

void interpret(Value *tree) {
    //function that calls "eval" for each top-level S-expression in the program.
    //print out any necesary results before moving to the next S-expression
    Value *curr = tree;

    //create a new frame for each new expression
    Frame *frame = talloc(sizeof(Frame));
    frame->bindings = makeNull();
    frame->parent = NULL;

    //bindings of primitives
    bind("+", &primitiveAdd, frame);
    bind("-", &primitiveMinus, frame);
    bind("/", &primitiveDivide, frame);
    bind("*", &primitiveMult, frame);
    bind(">", &primitiveGreater, frame);
    bind("<", &primitiveLess, frame);
    bind("=", &primitiveEqual, frame);
    bind("null?", &primitiveNull, frame);
    bind("car", &primitiveCar, frame);
    bind("cdr", &primitiveCdr, frame);
    bind("cons", &primitiveCons, frame);
    bind("modulo", &primitiveModulo, frame);
    
    //call eval with the subtree part that needs to be evaluated
    while (curr != NULL && curr->type == CONS_TYPE) {
        Value *result = eval(car(curr), frame);
        displayResults(result);
        printf("\n");
        curr = cdr(curr);
    }
}

Value *lookUpSymbol(Value *sym, Frame *frame) {
    Frame *currFrame = frame;
    while (currFrame != NULL) {
        Value *bindings = currFrame->bindings;
        while (bindings->type != NULL_TYPE) {
            Value *binding = car(bindings);
            //printf("binding is %s \n", car(binding)->s);
            if (strcmp(sym->s, car(binding)->s) == 0) {
                return cdr(binding);
            }
            bindings = cdr(bindings);
        }
        currFrame = currFrame->parent;
    }
    printf("Evaluation error: there is no binding for this symbol: %s \n", sym->s);
    texit(1);
    return NULL;
}

Value *evalIf(Value *args, Frame *frame) {
    //check if statement is legit
    if (args->type == NULL_TYPE || cdr(args)->type == NULL_TYPE || cdr(cdr(args))->type == NULL_TYPE) {
        printf("Evaluation error: if statements need to have 3 arguments\n");
        texit(1);
    }
    //evaulate the condition
    Value *curr = eval(car(args), frame);
    if (curr->type == BOOL_TYPE) {
        if (!strcmp(curr->s,"#t")) {
        //return the first argument
        return eval(car(cdr(args)), frame);
    } else {
        //return the second argument
        return eval(car(cdr(cdr(args))), frame);
    }
    }
    else {
        printf("Evaluation error: if statement second argument doesn't evaluate to a boolean value\n");
        texit(1);
        return NULL;
    }
}

Value *evalLet(Value *args, Frame *frame) {
    //there should be 2 more inputs
    if (args->type == NULL_TYPE || cdr(args)->type == NULL_TYPE) {
        printf("Evaluation error: let statements need to have at least 2 arguments\n");
        texit(1);
    }

    Value *bindings = car(args);
    if (bindings->type != NULL_TYPE && bindings->type != CONS_TYPE) {
        printf("Evaluation error: let statement bindings need to be a list\n");
        texit(1);
    }

    //create a new frame with the parent frame as the original frame
    Frame *newFrame = talloc(sizeof(Frame));
    newFrame->parent = frame;
    newFrame->bindings = makeNull();

    if (bindings->type == CONS_TYPE) {
        while (bindings->type != NULL_TYPE) {
            Value *curBinding = car(bindings);
            if (curBinding->type != CONS_TYPE || car(curBinding)->type != SYMBOL_TYPE || cdr(curBinding)->type != CONS_TYPE) {
                printf("Evaluation error: let statement bindings need to be a symbol followed by a value\n");
                texit(1);
            }

            //check for duplicates
            Value *check = newFrame->bindings;
            while (check->type != NULL_TYPE) {
                if (!strcmp(car(car(check))->s, car(curBinding)->s)) {
                    //this is a duplicate
                    printf("Evaluation error: duplicate bindings\n");
                    texit(1);
                }
                check = cdr(check);
            }

            Value *newBinding = cons(car(curBinding), eval(car(cdr(curBinding)), frame));
            newFrame->bindings = cons(newBinding, newFrame->bindings);
            bindings = cdr(bindings);
        }
    }

    Value *curargs = cdr(args);
    while(!isNull(cdr(curargs)) && car(cdr(curargs))->type != NULL_TYPE){
        eval(car(curargs), newFrame);
        curargs = cdr(curargs);
    }
    return eval(car(curargs), newFrame);
}


Value *evalLetStar(Value *args, Frame *frame){
    //there should be 2 more inputs
    if (args->type == NULL_TYPE || cdr(args)->type == NULL_TYPE) {
        printf("Evaluation error: let* statements need to have at least 2 arguments\n");
        texit(1);
    }

    Value *bindings = car(args);
    if (bindings->type != NULL_TYPE && bindings->type != CONS_TYPE) {
        printf("Evaluation error: let* statement bindings need to be a list\n");
        texit(1);
    }

    //create a new frame with the parent frame as the original frame 
    //first frame
    Frame *priorFrame = frame;

    if (bindings->type == CONS_TYPE) {
        while (bindings->type != NULL_TYPE) {
            Value *curBinding = car(bindings);
            if (curBinding->type != CONS_TYPE || car(curBinding)->type != SYMBOL_TYPE || cdr(curBinding)->type != CONS_TYPE) {
                printf("Evaluation error: let* statement bindings need to be a symbol followed by a value\n");
                texit(1);
            }

            //duplicate check doesn't really work here, new frame every time

            //setting the new frame attributes after checking the binding
            Frame *newFrame = talloc(sizeof(Frame)); 
            newFrame->parent = priorFrame;
            newFrame->bindings = makeNull();

            //eval the binding in the priorFrame
            Value *newBinding = cons(car(curBinding), eval(car(cdr(curBinding)), priorFrame));
            newFrame->bindings = cons(newBinding, newFrame->bindings);
            bindings = cdr(bindings);
            priorFrame = newFrame;
        }
    }

    //evaluate the solution in the latest newFrame
    Value *curargs = cdr(args);
    while(!isNull(cdr(curargs)) && car(cdr(curargs))->type != NULL_TYPE){
        eval(car(curargs), priorFrame);
        curargs = cdr(curargs);
    }
    return eval(car(curargs), priorFrame);
}

Value *evalSetBang(Value *args, Frame *frame){
    Frame *curr_frame = frame;
    Value *param = car(args);
    Value *void_node = talloc(sizeof(Value));
    void_node->type = VOID_TYPE;
    while(curr_frame != NULL){
        Value *curr = curr_frame->bindings;
        while(!isNull(curr) && !isNull(car(curr))){
            if((car(curr)->type == CONS_TYPE) && !strcmp(car(car(curr))->s, param->s)){
                //parameter is same as the thing we are trying to set
                (car(curr)->c).cdr = eval(car(cdr(args)), frame);
                return void_node;
            }
            curr = cdr(curr);
        }
        curr_frame = curr_frame->parent;
    }
    //binding doesn't exist, just return void
    //set doesn't create a new variable
    return void_node;
}

Value *evalLetrec(Value *args, Frame *frame) {
    if (args->type == NULL_TYPE || cdr(args)->type == NULL_TYPE) {
        printf("Evaluation error: letrec statements need to have at least 2 arguments\n");
        texit(1);
    }

    Value *bindings = car(args);
    if (bindings->type != NULL_TYPE && bindings->type != CONS_TYPE) {
        printf("Evaluation error: letrec statement bindings need to be a list\n");
        texit(1);
    }

    //create a new frame with the parent frame as the original frame
    Frame *newFrame = talloc(sizeof(Frame));
    newFrame->parent = frame;
    newFrame->bindings = makeNull();

    //Important restriction: it must be possible to evaluate each of the expressions e1 ... en 
    //without evaluating a reference or assignment to any of the variables x1 ... xn
    //returns unspecified in cases where it doesn't follow the rule
    Value *unspec = makeNull();
    unspec->type = UNSPECIFIED_TYPE;

    //4 steps for evaluating letrec
    //(1) binds the variables x1 ... xn to new locations, each holding an “undefined” value, 
    for (Value *curr = bindings; !isNull(curr) && !isNull(car(curr)); curr = cdr(curr)) {
        Value *node = talloc(sizeof(Value));
        node->type = CONS_TYPE;
        node->c.car = car(car(curr));
        node->c.cdr = unspec;
        newFrame->bindings = cons(node, newFrame->bindings);
    }

    for (Value *curr = bindings; !isNull(curr) && !isNull(car(curr)); curr = cdr(curr)) {
        if (eval(car(cdr(car(curr))), newFrame)->type == UNSPECIFIED_TYPE) {
            printf("Evaluation error: cannot use functions that were just defined in letrec\n");
            texit(1);
        }
    }

    //(2) evaluates the expressions e1 ... en in some unspecified order,
    //The expressions e1 ... en and body are all evaluated in an environment that contains the bindings of the variables 
    //(3) assigns the variables to the resulting values, and 
    for (Value *curr = bindings; !isNull(curr) && !isNull(car(curr)); curr = cdr(curr)) {
        evalSetBang(cons(car(car(curr)), cons(car(cdr(car(curr))), makeNull())), newFrame);
    }

    Value *curargs = cdr(args);
    while (!isNull(cdr(curargs)) && car(cdr(curargs))->type != NULL_TYPE) {
        eval(car(curargs), newFrame);
        curargs = cdr(curargs);
    }
    //(4) evaluates the body
    return eval(car(curargs), newFrame);
}

Value *evalBegin(Value *args, Frame *frame){
    if(isNull(args) || isNull(car(args))){
        //nothing to return
        Value *void_node = talloc(sizeof(Value));
        void_node->type = VOID_TYPE;
        return void_node;
    }
    Value *curr = args;
    Value *result = makeNull();
    while(!isNull(curr) && !isNull(car(curr))){
        //running through and evaluating each
        result = eval(car(curr), frame);
        curr = cdr(curr);
    }
    //returning last
    return result;
}

Value *evalCond(Value *args, Frame *frame) {
    Value *result = talloc(sizeof(Value));
    result->type = VOID_TYPE;
    Value *conditions = args;

    while (conditions->type != NULL_TYPE) {
    Value *current = car(conditions);
    Value *check = car(current);

      if (check->type == SYMBOL_TYPE && !strcmp(check->s, "else")) { 
        //if the input doesn't have else
        if (cdr(conditions)->type == NULL_TYPE) {
            Value *body = cdr(current);
            while (cdr(body)->type != NULL_TYPE) { 
                //returns the first true
                eval(car(body), frame);
                body = cdr(body);
            }
            return eval(car(body), frame);
        } else {
            printf("Evaluation Error: else is not the last input. \n");
        }
    }

     if (!(eval(check, frame)->type == BOOL_TYPE && !strcmp(eval(check, frame)->s, "#f"))) {
        //evaluates the else
        Value *body = cdr(current);
        while (cdr(body)->type != NULL_TYPE) {
            eval(car(body), frame);
            body = cdr(body);
        }
        return eval(car(body), frame);
        }
        conditions = cdr(conditions);
    }
    return result;
}

Value *evalAnd(Value *args, Frame *frame){
    Value *ans = talloc(sizeof(Value));
    ans->type = BOOL_TYPE;
    ans->s = talloc(sizeof(char) * 3);
    Value *curr = args;
    while(!isNull(curr) && !isNull(car(curr))){
        Value* val = eval(car(curr), frame);
        if(!(val->type == BOOL_TYPE && !strcmp(val->s, "#t"))){
            //ran into a false, so return false
            memcpy(ans->s, "#f", strlen("#f")+1);
            return ans;
        }
        curr = cdr(curr);
    }
    memcpy(ans->s, "#t", strlen("#t")+1);
    return ans;
}

Value *evalOr(Value *args, Frame *frame){
    Value *ans = talloc(sizeof(Value));
    ans->type = BOOL_TYPE;
    ans->s = talloc(sizeof(char) * 3);
    Value *curr = args;
    while(!isNull(curr) && !isNull(car(curr))){
        Value* val = eval(car(curr), frame);
        if((val->type == BOOL_TYPE && !strcmp(val->s, "#t"))){
            //found a true, so return true
            memcpy(ans->s, "#t", strlen("#t")+1);
            return ans;
        }
        curr = cdr(curr);
    }
    memcpy(ans->s, "#f", strlen("#f")+1);
    return ans;
}

int checkDefineArgs(Value *args) {
    //helper function for define to check errors, can add more
    if (args->type != CONS_TYPE) {
        printf("Evaluation error: binding not CONS_TYPE\n");
        return 0;
    }
    if (car(args)->type != SYMBOL_TYPE) {
        printf("Evaluation error: Assigning value to none symbol type\n");
        return 0;
    }
    if (cdr(args)->type == NULL_TYPE) {
        printf("Evaluation error: no value following the symbol in define.\n");
        return 0;
    }
    return 1;
}

int checkLambdaArgs(Value *args) {
    //helper function to check lambda inputs, can add more
    if (length(args) < 2) {
        printf("Evaluation error: no code in lambda following parameters.\n");
        return 0;
    }
    Value *params = car(args);
    while (!isNull(params)) {
        if (car(params)->type != SYMBOL_TYPE) {
            printf("Evaluation error: formal parameters for lambda must be symbols. \n");
            return 0;
        }
        params = cdr(params);
    }
    return 1;
}

int checkDuplicateParams(Value *param, Value *paramNames) {
    //helper function to check for duplicates, one while loop
    Value *curr = paramNames;
    while (!isNull(curr)) {
        if (!strcmp(car(param)->s, car(curr)->s)) {
            printf("Evaluation error: Duplicate Values\n");
            return 1;
        }
        curr = cdr(curr);
    }
    return 0;
}

Value *evalDefine(Value *args, Frame *frame) {
   Value *void_node = talloc(sizeof(Value));
   void_node->type = VOID_TYPE;
   Value *curr = args;

   if (!checkDefineArgs(args)) {
    printf("Evaluation error: checkDefineArgs didn't pass\n");
    texit(1);
   }

   Value *node = talloc(sizeof(Value));
   node->type = CONS_TYPE;
   node->c.car = car(curr);

   if (car(cdr(curr))->type == CONS_TYPE && car(car(cdr(curr)))->type != SYMBOL_TYPE) {
      node->c.cdr = car(cdr(curr));
   } else {
      node->c.cdr = eval(car(cdr(curr)), frame);
   }

   Value *next = frame->bindings;
   frame->bindings = cons(node, next);
   return void_node;
}

Value *apply(Value *function, Value *args) {
    Value *params = function->cl.paramNames;
    Frame *newFrm = talloc(sizeof(struct Frame));
    newFrm->parent = function->cl.frame;
    newFrm->bindings = makeNull();
    if(function->type == PRIMITIVE_TYPE){
        return (function->pf)(args);
    }
    //if i add the above line it doesn't work
    while (!isNull(params) && !isNull(args)) {
        Value *node = cons(car(args), makeNull());
        evalDefine(cons(car(params), node), newFrm);
        params = cdr(params);
        args = cdr(args);
    }
    return eval(function->cl.functionCode, newFrm);
}

Value *evalLambda(Value *args, Frame *frame) {
    if (!checkLambdaArgs(args)) {
        texit(1);
    }

    Value *clVal = talloc(sizeof(Value));
    clVal->type = CLOSURE_TYPE;
    clVal->cl.functionCode = car(cdr(args));
    clVal->cl.frame = frame;

    Value *params = car(args);
    clVal->cl.paramNames = makeNull();
    while (!isNull(params)) {
        if (checkDuplicateParams(params, clVal->cl.paramNames)) {
            texit(1);
        }
        clVal->cl.paramNames = cons(car(params), clVal->cl.paramNames);
        params = cdr(params);
    }
    clVal->cl.paramNames = reverse(clVal->cl.paramNames);
    return clVal;
}

Value *evalEach(Value *args, Frame *frame) {
    Value *result = makeNull();
    while (!isNull(args)) {
        result = cons(eval(car(args), frame), result);
        args = cdr(args);
    }
    return reverse(result);
}

Value *eval(Value *tree, Frame *frame) {
    Value *result = makeNull();
   switch (tree->type)  {
    case NULL_TYPE:{
        return tree;
    }
    case INT_TYPE:{
        return tree;
        break;
    }
    case BOOL_TYPE:{
        return tree;
        break;
    }
    case DOUBLE_TYPE:{
        return tree;
        break;
    }
    case STR_TYPE: {
        return tree;
        break;
    }
    case SYMBOL_TYPE: {
        result = lookUpSymbol(tree, frame);
        return result;
        break;
     }  
    case CLOSURE_TYPE:{
        return tree;
        break;
    }
    case PRIMITIVE_TYPE: {
        return tree;
        break;
    }
    case CONS_TYPE: {
        if(isNull(car(tree))){
            return tree;
        }
        Value *first = car(tree);
        Value *args = cdr(tree);
        // Sanity and error checking on first...
        if (isNull(first)) {
            //first token not a symbol type 
            printf("Evaluation error: incorrect type to apply\n");
            texit(1);
        } else if (!strcmp(first->s,"if")) {
            result = evalIf(args,frame);
        } else if (!strcmp(first->s,"let")) {
            result = evalLet(args, frame);
        } else if (!strcmp(first->s,"let*")) {
            result = evalLetStar(args, frame);
        } else if (!strcmp(first->s,"letrec")) {
            result = evalLetrec(args, frame);
        } else if (!strcmp(first->s,"quote")) {
            //return the rest of the expression 
            //as one thing
            if (args->type == NULL_TYPE) {
                printf("Evaluation error: quote needs an argument\n");
                texit(1);
            } else if (cdr(args)->type != NULL_TYPE) {
                printf("Evaluation error: too many arguments for quote\n");
                texit(1);
            } 
            return car(args);
        } else if (!strcmp(first->s,"define")) {
            result = evalDefine(args, frame);
        } else if (!strcmp(first->s,"lambda")) {
            result = evalLambda(args, frame);
        } else if (!strcmp(first->s, "set!")){
            result = evalSetBang(args, frame);
        } else if (!strcmp(first->s, "begin")){
            result = evalBegin(args, frame);
        } else if (!strcmp(first->s,"and")) {
            result = evalAnd(args, frame);
        } else if(!strcmp(first->s, "or")) {
            result = evalOr(args, frame);
        } else if(!strcmp(first->s, "cond")) {
            result = evalCond(args, frame);
        } else {
            Value *evaledOperator = eval(first, frame);
            Value *evaledArgs = evalEach(args, frame);
            return apply(evaledOperator,evaledArgs);
      }
      return result;
      break;
   }
   default:
      texit(1);
   } 
   printf("Evaluation error: wrong type\n");
   texit(1);
   return NULL;
}