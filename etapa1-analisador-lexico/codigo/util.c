#include "globals.h"
#include "util.h"

void printToken(TokenType token, const char *tokenString)
{
    switch (token) {
    case IF:
    case ELSE:
    case INT:
    case RETURN:
    case VOID:
    case WHILE:
        fprintf(listing, "palavra reservada: %s\n", tokenString);
        break;
    case ID:
        fprintf(listing, "ID, nome= %s\n", tokenString);
        break;
    case NUM:
        fprintf(listing, "NUM, valor= %s\n", tokenString);
        break;
    case ERROR:
        fprintf(listing, "ERRO: '%s'\n", tokenString);
        break;
    default:
        fprintf(listing, "%s\n", tokenString);
        break;
    }
}

void printSource(void)
{
    int c, n = 1, lineStart = TRUE;

    while ((c = fgetc(source)) != EOF) {
        if (lineStart)
            fprintf(listing, "  %3d  ", n++);
        fputc(c, listing);
        lineStart = (c == '\n');
    }
    if (!lineStart)
        fprintf(listing, "\n");
    rewind(source);
}
