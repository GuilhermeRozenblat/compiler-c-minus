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

// ENDFILE vale zero porque o yylex devolve 0 no fim do arquivo
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

// EchoSource mostra o codigo fonte com as linhas numeradas
extern int EchoSource;

// TraceScan mostra cada token reconhecido
extern int TraceScan;

// TraceStates mostra as transicoes do DFA no scanner manual
extern int TraceStates;

#endif
