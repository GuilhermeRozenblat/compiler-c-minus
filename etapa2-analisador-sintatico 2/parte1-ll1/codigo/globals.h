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

#define MAXEXPLEN 1000

// os nove primeiros sao os terminais, na ordem das colunas da tabela LL(1)
typedef enum {
    PLUS, MINUS, TIMES, OVER, POWER, LPAREN, RPAREN, NUM, ENDFILE,
    ERROR
} TokenType;

typedef struct {
    TokenType type;
    const char *string;     // aponta para dentro da expressao
    int length;
    int column;
} Token;

#define MAXCHILDREN 3

typedef struct treeNode {
    struct treeNode *child[MAXCHILDREN];
    int nchildren;
    int symbol;
    const Token *token;     // so nas folhas de num
} TreeNode;

extern FILE *listing;
extern char expression[MAXEXPLEN + 2];
extern Token tokens[MAXEXPLEN + 1];
extern int ntokens;

#endif
