#include "globals.h"
#include "util.h"
#include "scan.h"

#define TOPO "************************************************************\n"

FILE *source;
FILE *listing;
int lineno = 1;

int EchoSource = FALSE;
int TraceScan = TRUE;
int TraceStates = FALSE;

int main(int argc, char *argv[])
{
    char *pgm;

    if (argc == 3 && strcmp(argv[1], "--passos") == 0) {
        EchoSource = TRUE;
        TraceStates = TRUE;
    } else if (argc != 2) {
        fprintf(stderr, "uso: %s [--passos] arquivo.cm\n", argv[0]);
        exit(1);
    }
    pgm = argv[argc - 1];
    source = fopen(pgm, "r");
    if (source == NULL) {
        fprintf(stderr, "nao foi possivel abrir %s\n", pgm);
        exit(1);
    }
    listing = stdout;

    if (EchoSource) {
        fprintf(listing, TOPO "Arquivo: %s\n" TOPO "\nCodigo fonte:\n", pgm);
        printSource();
        fprintf(listing, "\n" TOPO "Etapa 1: analise lexica\n" TOPO "\nTokens:\n");
    }
    while (getToken() != ENDFILE)
        ;
    fclose(source);
    return 0;
}
