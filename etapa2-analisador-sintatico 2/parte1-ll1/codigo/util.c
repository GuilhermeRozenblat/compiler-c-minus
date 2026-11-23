#include <stdarg.h>
#include "globals.h"
#include "parse.h"
#include "util.h"

#define LINHA "____________________________________________________________\n"
#define STACKWIDTH 26

static const char *tokenName[] = {
    "MAIS", "MENOS", "VEZES", "DIVIDIDO", "POTENCIA", "ABRE", "FECHA", "NUM", "FIM"
};

// largura da coluna Entrada nos passos: a cadeia inteira de tokens
static int inputWidth;

TreeNode *newNode(int symbol)
{
    TreeNode *t = calloc(1, sizeof(TreeNode));

    if (t == NULL) {
        fprintf(stderr, "sem memoria\n");
        exit(2);
    }
    t->symbol = symbol;
    return t;
}

const char *productionString(int n)
{
    static char s[32];
    const Production *p = &production[n];
    int len, k;

    len = snprintf(s, sizeof s, "%s ->", symbolName[p->lhs]);
    if (p->length == 0)
        snprintf(s + len, sizeof s - len, " %s", symbolName[EMPTY]);
    for (k = 0; k < p->length; k++)
        len += snprintf(s + len, sizeof s - len, " %s", symbolName[p->rhs[k]]);
    return s;
}

void printToken(const Token *token)
{
    fprintf(listing, "  %-10.*s %-9s coluna %d\n",
            token->length, token->string, tokenName[token->type], token->column);
}

void printTable(void)
{
    int i, nt, t;

    fprintf(listing, "Producoes:\n");
    for (i = 1; i <= NPRODUCTIONS; i++)
        fprintf(listing, "  %2d  %s\n", i, productionString(i));

    fprintf(listing, "\nTabela LL(1):\n  %-7s", "");
    for (t = 0; t < NTERMINALS; t++)
        fprintf(listing, "%-7s", symbolName[t]);
    fprintf(listing, "\n");
    for (nt = 0; nt < NNONTERMINALS; nt++) {
        fprintf(listing, "  %-7s", symbolName[NTERMINALS + nt]);
        for (t = 0; t < NTERMINALS; t++) {
            if (parseTable[nt][t] == POP)
                fprintf(listing, "%-7s", "pop");
            else if (parseTable[nt][t] == SCAN)
                fprintf(listing, "%-7s", "scan");
            else
                fprintf(listing, "%-7d", parseTable[nt][t]);
        }
        fprintf(listing, "\n");
    }

    fprintf(listing, "\nErros:\n"
                     "  E1  operando esperado (linhas E, T, F e P)\n"
                     "  E2  operador ou fim esperado (linhas E', T' e F')\n"
                     "  E3  \")\" esperado (terminal no topo da pilha diferente do token)\n"
                     "  E4  simbolos sobrando depois do fim da expressao (pilha vazia)\n"
                     "  pop  desempilha o nao-terminal do topo\n"
                     "  scan descarta o token da entrada\n");
}

void printStepHeader(void)
{
    int k;

    inputWidth = 0;
    for (k = 0; k < ntokens; k++)
        inputWidth += tokens[k].length + (k > 0);
    if (inputWidth < 7)
        inputWidth = 7;
    fprintf(listing, "  %5s  %-*s  %*s  %s\n" LINHA,
            "Passo", STACKWIDTH, "Pilha", inputWidth, "Entrada", "Acao");
}

// uma linha da tabela de passos: a pilha, a entrada a partir do token pos e a acao
void printStep(int pos, const char *format, ...)
{
    static int nsteps = 0;
    va_list args;
    int k, width = 0;

    fprintf(listing, "  %5d  ", ++nsteps);
    for (k = 0; k < top; k++)
        width += fprintf(listing, "%s%s", k > 0 ? " " : "", symbolName[parseStack[k].symbol]);
    fprintf(listing, "%*s  ", width < STACKWIDTH ? STACKWIDTH - width : 0, "");

    width = 0;
    for (k = pos; k < ntokens; k++)
        width += tokens[k].length + (k > pos);
    fprintf(listing, "%*s", inputWidth - width, "");
    for (k = pos; k < ntokens; k++)
        fprintf(listing, "%s%.*s", k > pos ? " " : "", tokens[k].length, tokens[k].string);

    fprintf(listing, "  ");
    va_start(args, format);
    vfprintf(listing, format, args);
    va_end(args);
    fprintf(listing, "\n" LINHA);
}

// com um nao-terminal no topo, o esperado sao as colunas em que a linha tem producao
static void printExpected(int symbol)
{
    const int *row;
    int t, total = 0, seen = 0;

    if (symbol == ENDFILE) {
        fprintf(listing, "fim da expressao");
        return;
    }
    if (symbol < NTERMINALS) {
        fprintf(listing, "\"%s\"", symbolName[symbol]);
        return;
    }

    row = parseTable[symbol - NTERMINALS];
    for (t = 0; t < NTERMINALS; t++)
        if (row[t] > 0)
            total++;
    for (t = 0; t < NTERMINALS; t++) {
        if (row[t] <= 0)
            continue;
        if (++seen > 1)
            fprintf(listing, "%s", seen == total ? " ou " : ", ");
        if (t == ENDFILE)
            fprintf(listing, "fim da expressao");
        else if (t == NUM)
            fprintf(listing, "num");
        else
            fprintf(listing, "\"%s\"", symbolName[t]);
    }
}

void printErrors(void)
{
    int k;

    fprintf(listing, "\nErros:\n");
    for (k = 0; k < nerrors && k < MAXERRORS; k++) {
        const Token *t = errors[k].token;

        fprintf(listing, "  %s na coluna %d: encontrado ", errorType(errors[k].symbol), t->column);
        if (t->type == ENDFILE)
            fprintf(listing, "fim da expressao");
        else
            fprintf(listing, "\"%.*s\"", t->length, t->string);
        fprintf(listing, ", esperado ");
        printExpected(errors[k].symbol);
        fprintf(listing, "\n");
    }
}

static int indentno = 0;

#define INDENT indentno += 2
#define UNINDENT indentno -= 2

static void printSpaces(void)
{
    fprintf(listing, "%*s", indentno, "");
}

void printTree(TreeNode *tree)
{
    int i;

    printSpaces();
    if (tree->token != NULL)
        fprintf(listing, "num (%.*s)\n", tree->token->length, tree->token->string);
    else
        fprintf(listing, "%s\n", symbolName[tree->symbol]);
    INDENT;
    for (i = 0; i < tree->nchildren; i++)
        printTree(tree->child[i]);
    UNINDENT;
}
