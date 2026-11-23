%{
#include "globals.h"
#include "util.h"
#include "scan.h"

char tokenString[MAXTOKENLEN + 1];

// copia o lexema antes de cada acao
#define YY_USER_ACTION strncpy(tokenString, yytext, MAXTOKENLEN);
%}

%option noyywrap nounput

digit       [0-9]
number      {digit}+
letter      [a-zA-Z]
identifier  {letter}+
newline     \n
whitespace  [ \t\r]+

%%

"else"          { return ELSE; }
"if"            { return IF; }
"int"           { return INT; }
"return"        { return RETURN; }
"void"          { return VOID; }
"while"         { return WHILE; }
"+"             { return PLUS; }
"-"             { return MINUS; }
"*"             { return TIMES; }
"/"             { return OVER; }
"<"             { return LT; }
"<="            { return LE; }
">"             { return GT; }
">="            { return GE; }
"=="            { return EQ; }
"!="            { return NE; }
"="             { return ASSIGN; }
";"             { return SEMI; }
","             { return COMMA; }
"("             { return LPAREN; }
")"             { return RPAREN; }
"["             { return LBRACKET; }
"]"             { return RBRACKET; }
"{"             { return LBRACE; }
"}"             { return RBRACE; }
{number}        { return NUM; }
{identifier}    { return ID; }
{newline}       { lineno++; }
{whitespace}    { }
"/*"            { int c = 0, prev, startLine = lineno;
                  do {
                      prev = c;
                      c = input();
                      if (c == '\n')
                          lineno++;
                      if (c == 0 || c == EOF) {
                          // comentario nao fechado
                          lineno = startLine;
                          return ERROR;
                      }
                  } while (!(prev == '*' && c == '/'));
                }
.               { return ERROR; }

%%

TokenType getToken(void)
{
    static int firstTime = TRUE;
    TokenType currentToken;

    if (firstTime) {
        firstTime = FALSE;
        yyin = source;
        yyout = listing;
    }
    currentToken = yylex();
    if (TraceScan && currentToken != ENDFILE) {
        fprintf(listing, "%d: ", lineno);
        printToken(currentToken, tokenString);
    }
    return currentToken;
}
