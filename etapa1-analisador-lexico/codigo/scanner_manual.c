#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "scanner.h"

char lexema[TAMANHO_MAXIMO_LEXEMA + 1];
int linha = 1;

static FILE *fonte;

enum EstadoScanner {
    INICIO, EM_NUMERO, EM_IDENTIFICADOR,
    EM_MENOR, EM_MAIOR, EM_IGUAL, EM_EXCLAMACAO, PRONTO
};

/* so as quatro do subconjunto: int, void, while e return */
static const struct {
    const char *palavra;
    TipoToken token;
} reservadas[] = {
    {"int", INT},
    {"void", VOID},
    {"while", WHILE},
    {"return", RETURN}
};

void iniciar_scanner(FILE *arquivo)
{
    fonte = arquivo;
}

static int proximo_caractere(void)
{
    int c = fgetc(fonte);

    if (c == '\n')
        linha++;
    return c;
}

static void devolver_caractere(int c)
{
    if (c == EOF)
        return;
    if (c == '\n')
        linha--;
    ungetc(c, fonte);
}

static TipoToken buscar_palavra_reservada(const char *texto)
{
    for (size_t i = 0; i < sizeof(reservadas) / sizeof(reservadas[0]); i++)
        if (strcmp(reservadas[i].palavra, texto) == 0)
            return reservadas[i].token;
    return ID;
}

TipoToken proximo_token(void)
{
    enum EstadoScanner estado = INICIO;
    TipoToken token = ERRO;
    int tamanho = 0;

    while (estado != PRONTO) {
        int c = proximo_caractere();
        int acumula = 1;

        switch (estado) {
        case INICIO:
            if (isdigit(c))
                estado = EM_NUMERO;
            else if (isalpha(c))
                estado = EM_IDENTIFICADOR;
            else if (c == '<')
                estado = EM_MENOR;
            else if (c == '>')
                estado = EM_MAIOR;
            else if (c == '=')
                estado = EM_IGUAL;
            else if (c == '!')
                estado = EM_EXCLAMACAO;
            else if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
                acumula = 0;
            else {
                estado = PRONTO;
                switch (c) {
                case EOF:
                    acumula = 0;
                    token = FIM_ARQUIVO;
                    break;
                case ';':
                    token = PONTO_VIRGULA;
                    break;
                case '[':
                    token = ABRE_COLCHETE;
                    break;
                case ']':
                    token = FECHA_COLCHETE;
                    break;
                case '(':
                    token = ABRE_PARENTESE;
                    break;
                case ')':
                    token = FECHA_PARENTESE;
                    break;
                default:
                    token = ERRO;
                    break;
                }
            }
            break;

        case EM_NUMERO:
            if (!isdigit(c)) {
                devolver_caractere(c);
                acumula = 0;
                estado = PRONTO;
                token = NUM;
            }
            break;

        case EM_IDENTIFICADOR:
            if (!isalpha(c)) {
                devolver_caractere(c);
                acumula = 0;
                estado = PRONTO;
                token = ID;
            }
            break;

        case EM_MENOR:
            estado = PRONTO;
            if (c == '=')
                token = MENOR_IGUAL;
            else {
                devolver_caractere(c);
                acumula = 0;
                token = MENOR;
            }
            break;

        case EM_MAIOR:
            estado = PRONTO;
            if (c == '=')
                token = MAIOR_IGUAL;
            else {
                devolver_caractere(c);
                acumula = 0;
                token = MAIOR;
            }
            break;

        case EM_IGUAL:
            estado = PRONTO;
            if (c == '=')
                token = IGUAL;
            else {
                devolver_caractere(c);
                acumula = 0;
                token = ATRIBUICAO;
            }
            break;

        case EM_EXCLAMACAO:
            estado = PRONTO;
            if (c == '=')
                token = DIFERENTE;
            else {
                devolver_caractere(c);
                acumula = 0;
                token = ERRO;
            }
            break;

        case PRONTO:
            break;
        }

        if (acumula && tamanho < TAMANHO_MAXIMO_LEXEMA)
            lexema[tamanho++] = (char) c;
    }

    lexema[tamanho] = '\0';
    if (token == ID)
        token = buscar_palavra_reservada(lexema);
    return token;
}
