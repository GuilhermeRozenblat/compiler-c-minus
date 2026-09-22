#include <stdio.h>
#include "scanner.h"

static void imprimir_token(TipoToken token)
{
    printf("%d: ", linha);
    switch (token) {
    case IF:
    case ELSE:
    case INT:
    case RETURN:
    case VOID:
    case WHILE:
        printf("palavra reservada: %s\n", lexema);
        break;
    case ID:
        printf("ID, nome= %s\n", lexema);
        break;
    case NUM:
        printf("NUM, valor= %s\n", lexema);
        break;
    case ERRO:
        printf("ERRO: '%s'\n", lexema);
        break;
    default:
        printf("%s\n", lexema);
        break;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "uso: %s arquivo.cm\n", argv[0]);
        return 1;
    }

    FILE *fonte = fopen(argv[1], "r");
    if (fonte == NULL) {
        fprintf(stderr, "nao foi possivel abrir %s\n", argv[1]);
        return 1;
    }

    iniciar_scanner(fonte);
    TipoToken token;
    while ((token = proximo_token()) != FIM_ARQUIVO)
        imprimir_token(token);

    fclose(fonte);
    return 0;
}
