#include <stdarg.h>
#include "globals.h"
#include "util.h"
#include "scan.h"

#define LINHA "____________________________________________________________\n"

// o mesmo limite de profundidade da pilha do Bison (YYMAXDEPTH)
#define MAXSTACK 10000

void printToken(TokenType token, const char *tokenString)
{
    switch (token) {
    case ELSE:
    case IF:
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
    default:
        fprintf(listing, "%s\n", tokenString);
        break;
    }
}

// mostra o arquivo inteiro e volta ao comeco dele, para o scanner
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

static void *allocate(size_t size)
{
    void *p = calloc(1, size);

    if (p == NULL) {
        fprintf(stderr, "sem memoria\n");
        exit(2);
    }
    return p;
}

char *copyString(const char *s)
{
    char *t = allocate(strlen(s) + 1);

    strcpy(t, s);
    return t;
}

// os nos nao sao liberados: o programa mostra a arvore e termina
TreeNode *newNode(NodeKind kind)
{
    TreeNode *t = allocate(sizeof(TreeNode));

    t->kind = kind;
    return t;
}

TreeNode *newDeclNode(NodeKind kind, const char *type, const char *name)
{
    TreeNode *t = newNode(kind);

    t->type = type;
    t->name = name;
    return t;
}

TreeNode *newOpNode(const char *op, TreeNode *left, TreeNode *right)
{
    TreeNode *t = newNode(OpK);

    t->name = op;
    t->child[0] = left;
    t->child[1] = right;
    return t;
}

// poe t no fim da lista, que pode estar vazia
TreeNode *addSibling(TreeNode *list, TreeNode *t)
{
    TreeNode *last = list;

    if (list == NULL)
        return t;
    while (last->sibling != NULL)
        last = last->sibling;
    last->sibling = t;
    return list;
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

    while (tree != NULL) {
        printSpaces();
        switch (tree->kind) {
        case VarK:
            fprintf(listing, "Variavel: %s (%s)\n", tree->name, tree->type);
            break;
        case ArrayK:
            fprintf(listing, "Vetor: %s[%d] (%s)\n", tree->name, tree->val, tree->type);
            break;
        case FunK:
            fprintf(listing, "Funcao: %s (%s)\n", tree->name, tree->type);
            break;
        case ParamK:
            fprintf(listing, "Parametro: %s (%s)\n", tree->name, tree->type);
            break;
        case ArrayParamK:
            fprintf(listing, "Parametro vetor: %s[] (%s)\n", tree->name, tree->type);
            break;
        case CompoundK:
            fprintf(listing, "Bloco\n");
            break;
        case IfK:
            fprintf(listing, "Se\n");
            break;
        case WhileK:
            fprintf(listing, "Enquanto\n");
            break;
        case ReturnK:
            fprintf(listing, "Retorno\n");
            break;
        case EmptyK:
            fprintf(listing, "Comando vazio\n");
            break;
        case AssignK:
            fprintf(listing, "Atribuicao\n");
            break;
        case OpK:
            fprintf(listing, "Op: %s\n", tree->name);
            break;
        case ConstK:
            fprintf(listing, "Num: %d\n", tree->val);
            break;
        case IdK:
            fprintf(listing, "Id: %s\n", tree->name);
            break;
        case IndexK:
            fprintf(listing, "Indice: %s\n", tree->name);
            break;
        case CallK:
            fprintf(listing, "Chamada: %s\n", tree->name);
            break;
        }
        INDENT;
        for (i = 0; i < MAXCHILDREN; i++)
            printTree(tree->child[i]);
        UNINDENT;
        tree = tree->sibling;
    }
}

// o fim do arquivo e "$end" ate o Bison 3.5 e "end of file" do 3.6 em diante
static int isEndfile(const char *s, size_t len)
{
    return (len == 4 && strncmp(s, "$end", 4) == 0) ||
           (len == 11 && strncmp(s, "end of file", 11) == 0);
}

// alguns nomes ja vem entre aspas na mensagem do Bison
static void printTokenName(const char *s, size_t len)
{
    if (len >= 2 && s[0] == '"' && s[len - 1] == '"') {
        s++;
        len -= 2;
    }
    if (isEndfile(s, len))
        fprintf(listing, "fim do arquivo");
    else if (len == 13 && strncmp(s, "identificador", 13) == 0)
        fprintf(listing, "identificador");
    else if (len == 6 && strncmp(s, "numero", 6) == 0)
        fprintf(listing, "numero");
    else
        fprintf(listing, "\"%.*s\"", (int) len, s);
}

// traduz "syntax error, unexpected X, expecting A or B or C". O corte e feito
// nos trechos fixos, porque a virgula tambem e um token.
void printSyntaxError(const char *message)
{
    static const char UNEXPECTED[] = "syntax error, unexpected ";
    static const char EXPECTING[] = ", expecting ";
    static const char OR[] = " or ";
    const char *found, *expected, *item;
    size_t foundLen;
    int atEnd;

    if (strncmp(message, UNEXPECTED, sizeof UNEXPECTED - 1) != 0) {
        fprintf(listing, "Erro sintatico na linha %d: %s\n", lineno, message);
        return;
    }

    found = message + sizeof UNEXPECTED - 1;
    expected = strstr(found, EXPECTING);
    foundLen = expected ? (size_t) (expected - found) : strlen(found);
    atEnd = isEndfile(found, foundLen);

    if (atEnd)
        fprintf(listing, "Erro sintatico no fim do arquivo: ");
    else
        fprintf(listing, "Erro sintatico na linha %d: ", lineno);

    if (expected == NULL) {
        if (atEnd)
            fprintf(listing, "o codigo termina antes da hora\n");
        else {
            printTokenName(found, foundLen);
            fprintf(listing, " inesperado\n");
        }
        return;
    }

    // o erro aparece no token seguinte ao que falta, dai o "antes de"
    fprintf(listing, "esperado ");
    item = expected + sizeof EXPECTING - 1;
    for (;;) {
        const char *next = strstr(item, OR);

        printTokenName(item, next ? (size_t) (next - item) : strlen(item));
        if (next == NULL)
            break;
        item = next + sizeof OR - 1;
        fputs(strstr(item, OR) ? ", " : " ou ", listing);
    }
    if (!atEnd) {
        fprintf(listing, " antes de ");
        printTokenName(found, foundLen);
    }
    fprintf(listing, "\n");
}

// pilha de simbolos do --passos (a do Bison guarda estados)
static const char *symbolStack[MAXSTACK];
static int top;
static const char *pending;     // token recebido e ainda nao empilhado

void printStep(const char *format, ...)
{
    static int nsteps = 0;
    char action[100];
    va_list args;
    int k;

    va_start(args, format);
    vsnprintf(action, sizeof action, format, args);
    va_end(args);

    fprintf(listing, "  %5d  %-58s ", ++nsteps, action);
    for (k = 0; k < top; k++)
        fprintf(listing, " %s", symbolStack[k]);
    fprintf(listing, "\n" LINHA);
}

void printShift(void)
{
    if (pending == NULL)
        return;
    printStep("empilha %s", pending);
    symbolStack[top++] = pending;
    pending = NULL;
}

void printReceive(TokenType token)
{
    if (token == ENDFILE) {
        printStep("recebe fim do arquivo");
        return;
    }
    printStep("recebe %s (linha %d)", tokenString, lineno);
    pending = token == ID ? "ID" : token == NUM ? "NUM" : copyString(tokenString);
}

void printReduce(const char *lhs, const char *rhs, int noLookahead)
{
    const char *p;
    int length = 0;

    if (noLookahead)
        printShift();

    if (strcmp(rhs, "vazio") != 0)
        length = 1;
    for (p = rhs; *p != '\0'; p++)
        if (*p == ' ')
            length++;

    printStep("reduz %s -> %s", lhs, rhs);
    top -= length;
    symbolStack[top++] = lhs;
}
