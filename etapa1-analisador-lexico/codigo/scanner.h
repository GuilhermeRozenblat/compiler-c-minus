#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>

/* FIM_ARQUIVO deve ser zero: o yylex devolve 0 no fim do arquivo */
typedef enum {
    FIM_ARQUIVO = 0, ERRO,
    IF, ELSE, INT, RETURN, VOID, WHILE,
    ID, NUM,
    MAIS, MENOS, VEZES, DIVIDIDO,
    MENOR, MENOR_IGUAL, MAIOR, MAIOR_IGUAL, IGUAL, DIFERENTE,
    ATRIBUICAO,
    PONTO_VIRGULA, VIRGULA,
    ABRE_PARENTESE, FECHA_PARENTESE,
    ABRE_COLCHETE, FECHA_COLCHETE,
    ABRE_CHAVE, FECHA_CHAVE
} TipoToken;

extern char *lexema;
extern int linha;

void iniciar_scanner(FILE *arquivo);
TipoToken proximo_token(void);

#endif
