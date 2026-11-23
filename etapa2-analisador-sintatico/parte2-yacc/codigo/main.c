#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

#define TOPO  "************************************************************\n"
#define LINHA "____________________________________________________________\n"

FILE *source;
FILE *listing;
int lineno = 1;
int nerrors = 0;

int EchoSource = FALSE;
int TraceScan = FALSE;
int TraceParse = FALSE;

// do Flex
void yyrestart(FILE *file);

int main(int argc, char *argv[])
{
    char *pgm = NULL;
    TreeNode *syntaxTree;
    int i;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--passos") == 0)
            EchoSource = TraceScan = TraceParse = TRUE;
        else
            pgm = argv[i];
    }
    if (pgm == NULL) {
        fprintf(stderr, "uso: %s [--passos] arquivo.cm\n", argv[0]);
        return 2;
    }
    source = fopen(pgm, "r");
    if (source == NULL) {
        fprintf(stderr, "nao foi possivel abrir %s\n", pgm);
        return 2;
    }
    listing = stdout;

    if (EchoSource) {
        fprintf(listing, TOPO "Arquivo: %s\n" TOPO "\nCodigo fonte:\n", pgm);
        printSource();
    }

    // lista os tokens e volta ao inicio do arquivo para o parser
    if (TraceScan) {
        fprintf(listing, "\n" TOPO "Etapa 1: analise lexica\n" TOPO "\nTokens:\n");
        while (getToken() != ENDFILE)
            ;
        rewind(source);
        yyrestart(source);
        lineno = 1;
        nerrors = 0;
        TraceScan = FALSE;
    }

    if (TraceParse)
        fprintf(listing, "\n" TOPO "Etapa 2: analise sintatica\n" TOPO "\nPassos:\n  %5s  %-58s  %s\n" LINHA,
                "Passo", "Acao", "Pilha");
    syntaxTree = parse();
    fclose(source);
    if (TraceParse) {
        if (syntaxTree != NULL)
            printStep("aceita");
        fprintf(listing, "\n" TOPO "Resultado\n" TOPO);
    }

    if (nerrors > 0) {
        fprintf(listing, "\nCodigo invalido, erros: %d\n", nerrors);
        return 1;
    }
    fprintf(listing, "%sCodigo valido\n\nArvore sintatica:\n", TraceParse ? "\n" : "");
    printTree(syntaxTree);
    return 0;
}
