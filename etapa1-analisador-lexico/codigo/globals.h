#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

// ENDFILE precisa ser 0, que e o que o yylex devolve no fim
typedef enum {
    ENDFILE, ERROR,
    IF, ELSE, INT, RETURN, VOID, WHILE,
    ID, NUM,
    PLUS, MINUS, TIMES, OVER,
    LT, LE, GT, GE, EQ, NE, ASSIGN,
    SEMI, COMMA,
    LPAREN, RPAREN, LBRACKET, RBRACKET, LBRACE, RBRACE
} TokenType;

extern FILE *source;
extern FILE *listing;
extern int lineno;

// opcoes de saida, ligadas no main.c
extern int EchoSource;
extern int TraceScan;
extern int TraceStates;

#endif
