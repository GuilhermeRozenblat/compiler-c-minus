#ifndef _PARSE_H_
#define _PARSE_H_

#define NTERMINALS 9

// nao-terminais, numerados depois dos terminais para caberem na mesma pilha
enum { NT_E = NTERMINALS, NT_EP, NT_T, NT_TP, NT_F, NT_FP, NT_P, NSYMBOLS };

#define NNONTERMINALS (NSYMBOLS - NTERMINALS)
#define EMPTY NSYMBOLS          // so aparece como folha da arvore
#define NPRODUCTIONS 13

// celulas de erro da tabela: POP desempilha o nao-terminal, SCAN descarta o token
enum { POP = -1, SCAN = -2 };

typedef struct {
    int lhs;
    int length;
    int rhs[3];
} Production;

typedef struct {
    int symbol;
    TreeNode *node;
} StackItem;

// o tipo do erro e o que era esperado saem do simbolo que estava no topo
typedef struct {
    int symbol;
    const Token *token;
} SyntaxError;

#define MAXERRORS 50

extern const char *symbolName[];
extern const Production production[];
extern const int parseTable[NNONTERMINALS][NTERMINALS];

extern StackItem parseStack[];
extern int top;

extern SyntaxError errors[MAXERRORS];
extern int nerrors;

TreeNode *parse(void);
const char *errorType(int symbol);

#endif
