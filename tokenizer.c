#include "tokenizer.h"
#include "value.h"
#include "linkedlist.h"
#include "talloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_SIZE 300

bool isbeta(char ch) {
    if (ch == '!' || ch == '$' || ch == '%' || ch == '&' || ch == '+' ||
        ch == '*' || ch == '/' || ch == ':' || ch == '<' || ch == '=' ||
        ch == '>' || ch == '?' || ch == '~' || ch == '_' || ch == '^' ||
        ch == '-') {
        return true;
    }
    return false;
}
Value *tokenize() {
    char c;
    Value *list = makeNull();
    
    while ((c = (char)fgetc(stdin)) != EOF) {
        Value *token = talloc(sizeof(Value));

        if (c == '(') { //open paren
            token->type = OPEN_TYPE;
        } else if (c == ')') { //close paren
            token->type = CLOSE_TYPE;
        //} else if (c == '[') { //ppen bracket
         //   token->type = OPENBRACKET_TYPE;
        //} else if (c == ']') { //close bracket
         //   token->type = CLOSEBRACKET_TYPE;
        } else if (isspace(c) != 0) { //spaces
            continue;
        } else if (c == '-') { //dealing with negative signs
            int negative = '-';
            if (isdigit(c = (char)fgetc(stdin))) {
                //c is now the first digit
                //need to add negative
                char num[MAX_SIZE] = {negative};
                num[1] = c;
                int i = 2;
                while (isdigit(c = (char)fgetc(stdin))) {
                    if (i < MAX_SIZE) {
                        num[i++] = c;
                    } else {
                        printf("Syntax error: number too long\n");
                    texit(1);
                }
            }
            if (c == '.') { //doubles more specifically
                num[i++] = c;
                while (isdigit(c = (char)fgetc(stdin))) {
                    if (i < MAX_SIZE) {
                        num[i++] = c;
                    } else {
                        printf("Syntax error: number too long\n");
                        texit(1);
                    }
                }
                ungetc(c, stdin);
                num[i] = '\0';
                token->d = atof(num);
                token->type = DOUBLE_TYPE;
            } else {
                ungetc(c, stdin);
                num[i] = '\0';
                token->i = atoi(num);
                token->type = INT_TYPE;
            }
            } else {
                token->type = SYMBOL_TYPE;
                char sym[3] = {negative};
                sym[2] = '\0';
                token->s = talloc(3*sizeof(char));
                strcpy(token->s, sym);
                ungetc(c, stdin);
            }
        } else if (c == '#') {  //boolean
            int d = (char)fgetc(stdin);
            if (d == 'f' || d == 'F') {
                token->s = "#f";
                token->type = BOOL_TYPE;
            } else if (d == 't' || d == 'T') {
                token->s = "#t";
                token->type = BOOL_TYPE;
            } else {
                printf("Syntax error: expected boolean value after #\n");
                texit(1);
            }
        } else if (c == ';') { //comments
            while (c != '\n') { //moving to the end of the line
                c = (char)fgetc(stdin);
            }
            continue;
        } else if (isbeta(c)) { //symbols, singular math operations except subtraction
            token->type = SYMBOL_TYPE;
            char sym[3] = {c};
            sym[2] = '\0';
            token->s = talloc(3*sizeof(char));
            strcpy(token->s, sym);
        } else if (isalpha(c) || c == '_') { //symbols
            char sym[MAX_SIZE] = {c};
            int i = 1;
            while (isalnum(c = (char)fgetc(stdin)) || c == '_' || c == '-' || isbeta(c)) { //handles things like n-1
                if (i < MAX_SIZE) {
                    sym[i++] = c;
                } else {
                    printf("Syntax error: symbol too long\n");
                    texit(1);
                }
            }
            ungetc(c, stdin);
            sym[i] = '\0';
            token->s = talloc((i + 1) * sizeof(char));
            strcpy(token->s, sym);
            token->type = SYMBOL_TYPE;
        } else if (c == '\"') { //handles strings
            char str[MAX_SIZE];
            int i = 0;
            while ((c = (char)fgetc(stdin)) != '\"') {
                if (c == EOF) {
                    printf("Syntax error: expected closing quote for string\n");
                    texit(1);
                } else if (i < MAX_SIZE - 1) {
                    str[i++] = c;
                } else {
                    printf("Syntax error: string too long\n");
                    texit(1);
                }
            }
            str[i] = '\0';
            token->s = talloc((i + 1) * sizeof(char));
            strcpy(token->s, str);
            token->type = STR_TYPE;
        } else if (isdigit(c)) { //doubles and ints, non-negative
            char num[MAX_SIZE] = {c};
            int i = 1;
            while (isdigit(c = (char)fgetc(stdin))) {
                if (i < MAX_SIZE) {
                    num[i++] = c;
                } else {
                    printf("Syntax error: number too long\n");
                    texit(1);
                }
            }
            if (c == '.') { //doubles more specifically
                num[i++] = c;
                while (isdigit(c = (char)fgetc(stdin))) {
                    if (i < MAX_SIZE) {
                        num[i++] = c;
                    } else {
                        printf("Syntax error: number too long\n");
                        texit(1);
                    }
                }
                ungetc(c, stdin);
                num[i] = '\0';
                token->d = atof(num);
                token->type = DOUBLE_TYPE;
            } else {
                ungetc(c, stdin);
                num[i] = '\0';
                token->i = atoi(num);
                token->type = INT_TYPE;
            }
        } else if (c == '.') { //more double handling, starts with .
            char next = (char)fgetc(stdin);
            if (isdigit(next)) {
                char num[MAX_SIZE] = {c, next};
                int i = 2;
                while (isdigit(c = (char)fgetc(stdin))) {
                    if (i < MAX_SIZE) {
                        num[i++] = c;
                    } else {
                        printf("Syntax error: number too long\n");
                        texit(1);
                    }
                }
                ungetc(c, stdin);
                num[i] = '\0';
                token->d = atof(num);
                token->type = DOUBLE_TYPE;
            } else {
                printf("Syntax error: expected digit after .\n");
                texit(1);
            }
        // } else if (c == '\'') { //single quote
        //     token->type = SINGLEQUOTE_TYPE;
        //     char sym[MAX_SIZE] = {c};
        //     int i = 1;
        //     if (isspace(c = (char)fgetc(stdin)) != 0) {
        //         printf("Syntax error: symbol ' does not start with an allowed first character\n");
        //         texit(1);
        //     } else {
        //         sym[i] = c;
        //         i++;
        //     }
        //     while (isspace(c = (char)fgetc(stdin)) == 0) {
        //         if (i < MAX_SIZE) {
        //                 num[i++] = c;
        //             } else {
        //                 printf("Syntax error: number too long\n");
        //             texit(1);
        //         }
        //     }
        //     token->s = talloc(3*sizeof(char));
        //     strcpy(token->s, sym);
        } else {
            // printf("this is c \'%c\'\n", c);
            // displayTokens(reverse(list));
            printf("Syntax error: invalid character %c\n", c);
            texit(1);
        }
        
        list = cons(token, list);
    }
    
    return reverse(list);
}


void displayTokens(Value *list) {
    Value *cur = list;
    while (!isNull(cur)) {
        
        switch (car(cur)->type) {
            case INT_TYPE:
                printf("%d:integer \n", car(cur)->i);
                break;
            case BOOL_TYPE:
                printf("%s:boolean \n", car(cur)->s);
                break;
            case DOUBLE_TYPE:
                printf("%f:double \n", car(cur)->d);
                break;
            case STR_TYPE:
                printf("\"%s\":string \n", car(cur)->s);
                break;
            case SYMBOL_TYPE:
                printf("%s:symbol \n", car(cur)->s);
                break;
            case OPEN_TYPE:
                printf("(:open \n");
                break;
            case CLOSE_TYPE:
                printf("):close \n");
                break;
            //case OPENBRACKET_TYPE:
            //    printf("[:openbracket \n");
            //    break;
            //case CLOSEBRACKET_TYPE:
            //    printf("]:closebracket \n");
            //    break;
            // case SINGLEQUOTE_TYPE:
            //     printf("\':singlequote \n");
            //     break;
            default:
                break;
        }
        cur = cdr(cur);
    }
}