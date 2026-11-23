#include "globals.h"
#include "util.h"
#include "scan.h"

#define MAXRESERVED 4

// estados do DFA minimo
typedef enum {
    START, INNUM, INID, INLT, INGT, INASSIGN, INNOT, DONE
} StateType;

static const char *stateName[] = {
    "START", "INNUM", "INID", "INLT", "INGT", "INASSIGN", "INNOT", "DONE"
};

char tokenString[MAXTOKENLEN + 1];

static struct {
    char *str;
    TokenType tok;
} reservedWords[MAXRESERVED] = {
    {"int", INT}, {"void", VOID}, {"while", WHILE}, {"return", RETURN}
};

static int getNextChar(void)
{
    int c = fgetc(source);

    if (c == '\n')
        lineno++;
    return c;
}

static void ungetNextChar(int c)
{
    if (c == EOF)
        return;
    if (c == '\n')
        lineno--;
    ungetc(c, source);
}

static TokenType reservedLookup(char *s)
{
    int i;

    for (i = 0; i < MAXRESERVED; i++)
        if (strcmp(s, reservedWords[i].str) == 0)
            return reservedWords[i].tok;
    return ID;
}

static void printTransition(int c, StateType from, StateType to, int save)
{
    if (save) {
        if (from == START)
            fprintf(listing, "____________________________________________________________\n");
        fprintf(listing, "  %c  %s -> %s\n", c, stateName[from], stateName[to]);
    } else if (from != START)
        fprintf(listing, "     %s -> %s, nao consome o caractere\n", stateName[from], stateName[to]);
}

TokenType getToken(void)
{
    int tokenStringIndex = 0;
    TokenType currentToken = ERROR;
    StateType state = START;
    int save;

    while (state != DONE) {
        int c = getNextChar();
        StateType oldState = state;

        save = TRUE;
        switch (state) {
        case START:
            if (isdigit(c))
                state = INNUM;
            else if (isalpha(c))
                state = INID;
            else if (c == '<')
                state = INLT;
            else if (c == '>')
                state = INGT;
            else if (c == '=')
                state = INASSIGN;
            else if (c == '!')
                state = INNOT;
            else if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
                save = FALSE;
            else {
                state = DONE;
                switch (c) {
                case EOF:
                    save = FALSE;
                    currentToken = ENDFILE;
                    break;
                case ';':
                    currentToken = SEMI;
                    break;
                case '[':
                    currentToken = LBRACKET;
                    break;
                case ']':
                    currentToken = RBRACKET;
                    break;
                case '(':
                    currentToken = LPAREN;
                    break;
                case ')':
                    currentToken = RPAREN;
                    break;
                default:
                    currentToken = ERROR;
                    break;
                }
            }
            break;
        case INNUM:
            if (!isdigit(c)) {
                ungetNextChar(c);
                save = FALSE;
                state = DONE;
                currentToken = NUM;
            }
            break;
        case INID:
            if (!isalpha(c)) {
                ungetNextChar(c);
                save = FALSE;
                state = DONE;
                currentToken = ID;
            }
            break;
        case INLT:
            state = DONE;
            if (c == '=')
                currentToken = LE;
            else {
                ungetNextChar(c);
                save = FALSE;
                currentToken = LT;
            }
            break;
        case INGT:
            state = DONE;
            if (c == '=')
                currentToken = GE;
            else {
                ungetNextChar(c);
                save = FALSE;
                currentToken = GT;
            }
            break;
        case INASSIGN:
            state = DONE;
            if (c == '=')
                currentToken = EQ;
            else {
                ungetNextChar(c);
                save = FALSE;
                currentToken = ASSIGN;
            }
            break;
        case INNOT:
            // "!" sem "=" e erro
            state = DONE;
            if (c == '=')
                currentToken = NE;
            else {
                ungetNextChar(c);
                save = FALSE;
                currentToken = ERROR;
            }
            break;
        case DONE:
            break;
        }
        if (TraceStates)
            printTransition(c, oldState, state, save);
        if (save && tokenStringIndex < MAXTOKENLEN)
            tokenString[tokenStringIndex++] = (char) c;
        if (state == DONE) {
            tokenString[tokenStringIndex] = '\0';
            if (currentToken == ID)
                currentToken = reservedLookup(tokenString);
        }
    }
    if (TraceScan && currentToken != ENDFILE) {
        fprintf(listing, "%d: ", lineno);
        printToken(currentToken, tokenString);
    }
    return currentToken;
}
