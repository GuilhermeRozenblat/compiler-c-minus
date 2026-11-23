#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// codigos dos tokens gerados pelo Bison (o parser ja os tem)
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

// erros lexicos e sintaticos
extern int nerrors;

typedef enum {
    VarK, ArrayK, FunK, ParamK, ArrayParamK,
    CompoundK, IfK, WhileK, ReturnK, EmptyK,
    AssignK, OpK, ConstK, IdK, IndexK, CallK
} NodeKind;

// filhos de cada no:
//   FunK       parametros, corpo
//   CompoundK  declaracoes locais, comandos
//   IfK        condicao, comando, comando do else
//   WhileK     condicao, comando
//   ReturnK    expressao
//   AssignK    variavel, expressao
//   OpK        operando esquerdo, operando direito
//   IndexK     expressao do indice
//   CallK      argumentos
// sibling encadeia declaracoes, parametros, comandos e argumentos
#define MAXCHILDREN 3

typedef struct treeNode {
    struct treeNode *child[MAXCHILDREN];
    struct treeNode *sibling;
    NodeKind kind;
    const char *name;   // identificador, ou o operador em OpK
    const char *type;   // "int" ou "void", nas declaracoes
    int val;            // valor em ConstK, tamanho em ArrayK
} TreeNode;

// EchoSource mostra o codigo fonte com as linhas numeradas
extern int EchoSource;

// TraceScan mostra cada token reconhecido
extern int TraceScan;

// TraceParse mostra cada passo da analise sintatica, com a pilha
extern int TraceParse;

#endif
