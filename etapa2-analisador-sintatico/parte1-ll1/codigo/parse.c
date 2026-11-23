#include "globals.h"
#include "util.h"
#include "parse.h"

// cada parentese aberto deixa 4 simbolos na pilha
#define MAXSTACK (4 * MAXEXPLEN + 16)

const char *symbolName[] = {
    "+", "-", "*", "/", "^", "(", ")", "num", "$",
    "E", "E'", "T", "T'", "F", "F'", "P",
    "vazio"
};

// mesma numeracao do relatorio (a 0 nao e usada)
const Production production[] = {
    { 0,     0, { 0 } },
    { NT_E,  2, { NT_T, NT_EP } },              //  1  E  -> T E'
    { NT_EP, 3, { PLUS, NT_T, NT_EP } },        //  2  E' -> + T E'
    { NT_EP, 3, { MINUS, NT_T, NT_EP } },       //  3  E' -> - T E'
    { NT_EP, 0, { 0 } },                        //  4  E' -> vazio
    { NT_T,  2, { NT_F, NT_TP } },              //  5  T  -> F T'
    { NT_TP, 3, { TIMES, NT_F, NT_TP } },       //  6  T' -> * F T'
    { NT_TP, 3, { OVER, NT_F, NT_TP } },        //  7  T' -> / F T'
    { NT_TP, 0, { 0 } },                        //  8  T' -> vazio
    { NT_F,  2, { NT_P, NT_FP } },              //  9  F  -> P F'
    { NT_FP, 2, { POWER, NT_F } },              // 10  F' -> ^ F
    { NT_FP, 0, { 0 } },                        // 11  F' -> vazio
    { NT_P,  3, { LPAREN, NT_E, RPAREN } },     // 12  P  -> ( E )
    { NT_P,  1, { NUM } }                       // 13  P  -> num
};

// numero positivo: a producao a aplicar
const int parseTable[NNONTERMINALS][NTERMINALS] = {
    //   +     -     *     /     ^     (     )   num     $
    { SCAN, SCAN, SCAN, SCAN, SCAN,    1,  POP,    1,  POP },     // E
    {    2,    3, SCAN, SCAN, SCAN, SCAN,    4, SCAN,    4 },     // E'
    {  POP,  POP, SCAN, SCAN, SCAN,    5,  POP,    5,  POP },     // T
    {    8,    8,    6,    7, SCAN, SCAN,    8, SCAN,    8 },     // T'
    {  POP,  POP,  POP,  POP, SCAN,    9,  POP,    9,  POP },     // F
    {   11,   11,   11,   11,   10, SCAN,   11, SCAN,   11 },     // F'
    {  POP,  POP,  POP,  POP,  POP,   12,  POP,   13,  POP }      // P
};

StackItem parseStack[MAXSTACK];
int top;

SyntaxError errors[MAXERRORS];
int nerrors;

static int recovering = FALSE;

static void push(int symbol, TreeNode *node)
{
    if (top == MAXSTACK) {
        fprintf(stderr, "expressao grande demais\n");
        exit(2);
    }
    parseStack[top].symbol = symbol;
    parseStack[top].node = node;
    top++;
}

// depois de um erro, o proximo so e informado apos um movimento correto,
// para nao sair em cascata (Louden, p. 186)
static void syntaxError(int symbol, const Token *token)
{
    if (recovering)
        return;
    recovering = TRUE;
    if (nerrors < MAXERRORS) {
        errors[nerrors].symbol = symbol;
        errors[nerrors].token = token;
    }
    nerrors++;
}

const char *errorType(int symbol)
{
    if (symbol == ENDFILE)
        return "E4";
    if (symbol < NTERMINALS)
        return "E3";
    if (symbol == NT_EP || symbol == NT_TP || symbol == NT_FP)
        return "E2";
    return "E1";
}

// algoritmo da Figura 4.2 do Louden, com a arvore montada junto com a pilha
TreeNode *parse(void)
{
    TreeNode *syntaxTree = newNode(NT_E);
    int i = 0;

    push(ENDFILE, NULL);
    push(NT_E, syntaxTree);

    for (;;) {
        StackItem X = parseStack[top - 1];
        const Token *a = &tokens[i];
        int cell, k;

        if (X.symbol == ENDFILE && a->type == ENDFILE) {
            printStep(i, nerrors == 0 ? "accept" : "fim (houve erro)");
            break;
        }

        if (X.symbol == ENDFILE) {
            syntaxError(X.symbol, a);
            printStep(i, "erro E4: simbolos sobrando, para");
            break;
        }

        if (X.symbol < NTERMINALS) {
            if (X.symbol == (int) a->type) {
                printStep(i, "match %s", symbolName[X.symbol]);
                recovering = FALSE;
                if (a->type == NUM)
                    X.node->token = a;
                top--;
                i++;
            } else {
                syntaxError(X.symbol, a);
                printStep(i, "erro E3: desempilha %s", symbolName[X.symbol]);
                top--;
            }
            continue;
        }

        cell = parseTable[X.symbol - NTERMINALS][a->type];
        if (cell > 0) {
            const Production *p = &production[cell];

            printStep(i, "%s", productionString(cell));
            recovering = FALSE;
            top--;
            if (p->length == 0)
                X.node->child[X.node->nchildren++] = newNode(EMPTY);
            for (k = 0; k < p->length; k++)
                X.node->child[X.node->nchildren++] = newNode(p->rhs[k]);
            for (k = p->length - 1; k >= 0; k--)
                push(p->rhs[k], X.node->child[k]);
            continue;
        }

        syntaxError(X.symbol, a);
        if (cell == SCAN) {
            printStep(i, "erro %s: scan (descarta %.*s)", errorType(X.symbol), a->length, a->string);
            i++;
        } else {
            printStep(i, "erro %s: pop (desempilha %s)", errorType(X.symbol), symbolName[X.symbol]);
            top--;
        }
    }
    return syntaxTree;
}
