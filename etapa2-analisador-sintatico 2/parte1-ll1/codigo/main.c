#include "globals.h"
#include "scan.h"
#include "parse.h"
#include "util.h"

#define TOPO "************************************************************\n"

FILE *listing;
char expression[MAXEXPLEN + 2];
Token tokens[MAXEXPLEN + 1];
int ntokens;

int main(int argc, char *argv[])
{
    FILE *source = stdin;
    TreeNode *syntaxTree;
    size_t len;

    listing = stdout;
    if (argc == 2 && strcmp(argv[1], "--tabela") == 0) {
        printTable();
        return 0;
    }
    if (argc > 2) {
        fprintf(stderr, "uso: %s [arquivo | --tabela]\n", argv[0]);
        return 2;
    }
    if (argc == 2 && (source = fopen(argv[1], "r")) == NULL) {
        fprintf(stderr, "nao foi possivel abrir %s\n", argv[1]);
        return 2;
    }

    len = fread(expression, 1, MAXEXPLEN + 1, source);
    if (source != stdin)
        fclose(source);
    if (len > MAXEXPLEN) {
        fprintf(stderr, "expressao grande demais\n");
        return 2;
    }
    // sem os espacos do fim, o fim da entrada fica logo depois do ultimo caractere
    while (len > 0 && strchr(" \t\r\n", expression[len - 1]) != NULL)
        len--;
    expression[len] = '\0';
    fprintf(listing, "Expressao: %s\n", len > 0 ? expression : "(vazia)");

    // i. analise lexica
    fprintf(listing, "\n" TOPO "Etapa 1: analise lexica\n" TOPO "\nTokens:\n");
    do {
        Token t = getToken();

        if (t.type == ERROR) {
            fprintf(listing, "\nErro lexico na coluna %d: %s \"%.*s\"\n", t.column,
                    isdigit((unsigned char) t.string[0]) ? "numero incompleto" : "caractere invalido",
                    t.length, t.string);
            fprintf(listing, "\n" TOPO "Resultado\n" TOPO "\nResultado: expressao rejeitada, erro lexico\n");
            return 1;
        }
        printToken(&t);
        tokens[ntokens++] = t;
    } while (tokens[ntokens - 1].type != ENDFILE);

    // ii e iii. analise sintatica pela tabela, mostrando os passos
    fprintf(listing, "\n" TOPO "Etapa 2: analise sintatica pela tabela LL(1)\n" TOPO "\nPassos:\n");
    printStepHeader();
    syntaxTree = parse();
    fprintf(listing, "\n" TOPO "Resultado\n" TOPO);

    if (nerrors > 0) {
        printErrors();
        fprintf(listing, "\nResultado: expressao rejeitada, erros: %d\n", nerrors);
        return 1;
    }

    // iv. arvore de analise sintatica
    fprintf(listing, "\nResultado: expressao aceita\n\nArvore:\n");
    printTree(syntaxTree);
    return 0;
}
