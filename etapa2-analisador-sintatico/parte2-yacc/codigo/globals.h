#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// tokens gerados pelo Bison
#ifndef YYPARSER
#include "cminus.tab.h"
#define ENDFILE 0
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

typedef int TokenType;

extern FILE *source;
extern FILE *listing;
extern int lineno;

extern int nerrors;

typedef enum {
    VarK, ArrayK, FunK, ParamK, ArrayParamK,
    CompoundK, IfK, WhileK, ReturnK, EmptyK,
    AssignK, OpK, ConstK, IdK, IndexK, CallK
} NodeKind;

// filhos: FunK (parametros, corpo), CompoundK (declaracoes, comandos),
// IfK (condicao, comando, else), WhileK (condicao, comando),
// AssignK (variavel, expressao), OpK (esquerdo, direito)
// sibling liga declaracoes, parametros, comandos e argumentos
#define MAXCHILDREN 3

typedef struct treeNode {
    struct treeNode *child[MAXCHILDREN];
    struct treeNode *sibling;
    NodeKind kind;
    const char *name;   // identificador, ou o operador em OpK
    const char *type;   // "int" ou "void", nas declaracoes
    int val;            // valor em ConstK, tamanho em ArrayK
} TreeNode;

// opcoes de saida, ligadas no main.c
extern int EchoSource;
extern int TraceScan;
extern int TraceParse;

#endif
