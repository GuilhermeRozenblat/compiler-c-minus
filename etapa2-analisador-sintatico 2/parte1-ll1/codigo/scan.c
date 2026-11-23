#include "globals.h"
#include "scan.h"

// NUM = digit+ (("." | ",") digit+)?
typedef enum { START, INNUM, INSEP, INFRAC, DONE } StateType;

static int linepos = 0;

Token getToken(void)
{
    Token token;
    StateType state = START;

    while (expression[linepos] == ' ' || expression[linepos] == '\t' ||
           expression[linepos] == '\r' || expression[linepos] == '\n')
        linepos++;
    token.type = ERROR;
    token.string = expression + linepos;
    token.column = linepos + 1;

    while (state != DONE) {
        int c = (unsigned char) expression[linepos];

        switch (state) {
        case START:
            if (isdigit(c)) {
                state = INNUM;
                linepos++;
                break;
            }
            state = DONE;
            switch (c) {
            case '\0': token.type = ENDFILE;               break;
            case '+':  token.type = PLUS;     linepos++;   break;
            case '-':  token.type = MINUS;    linepos++;   break;
            case '*':  token.type = TIMES;    linepos++;   break;
            case '/':  token.type = OVER;     linepos++;   break;
            case '^':  token.type = POWER;    linepos++;   break;
            case '(':  token.type = LPAREN;   linepos++;   break;
            case ')':  token.type = RPAREN;   linepos++;   break;
            default:
                // caractere invalido (se for UTF-8 de varios bytes, vai inteiro)
                linepos++;
                while ((expression[linepos] & 0xC0) == 0x80)
                    linepos++;
                break;
            }
            break;
        case INNUM:
            if (isdigit(c))
                linepos++;
            else if (c == '.' || c == ',') {
                state = INSEP;
                linepos++;
            } else {
                token.type = NUM;
                state = DONE;
            }
            break;
        case INSEP:
            // "3." sem digito depois continua ERROR
            if (isdigit(c)) {
                state = INFRAC;
                linepos++;
            } else
                state = DONE;
            break;
        case INFRAC:
            if (isdigit(c))
                linepos++;
            else {
                token.type = NUM;
                state = DONE;
            }
            break;
        case DONE:
            break;
        }
    }

    if (token.type == ENDFILE) {
        token.string = "$";
        token.length = 1;
    } else
        token.length = (int) (expression + linepos - token.string);
    return token;
}
