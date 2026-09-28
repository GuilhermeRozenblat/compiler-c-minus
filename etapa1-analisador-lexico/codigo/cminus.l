%{
#include "scanner.h"
#define YY_USER_ACTION lexema = yytext;
%}

%option noyywrap nounput

digito        [0-9]
numero        {digito}+
letra         [a-zA-Z]
identificador {letra}+

%%

"/*"            { int c = 0, anterior;
                  do {
                      anterior = c;
                      c = input();
                      if (c == '\n')
                          linha++;
                      if (c == 0) {
                          lexema = "/*";
                          return ERRO;
                      }
                  } while (!(anterior == '*' && c == '/'));
                }
"else"          { return ELSE; }
"if"            { return IF; }
"int"           { return INT; }
"return"        { return RETURN; }
"void"          { return VOID; }
"while"         { return WHILE; }
"+"             { return MAIS; }
"-"             { return MENOS; }
"*"             { return VEZES; }
"/"             { return DIVIDIDO; }
"<"             { return MENOR; }
"<="            { return MENOR_IGUAL; }
">"             { return MAIOR; }
">="            { return MAIOR_IGUAL; }
"=="            { return IGUAL; }
"!="            { return DIFERENTE; }
"="             { return ATRIBUICAO; }
";"             { return PONTO_VIRGULA; }
","             { return VIRGULA; }
"("             { return ABRE_PARENTESE; }
")"             { return FECHA_PARENTESE; }
"["             { return ABRE_COLCHETE; }
"]"             { return FECHA_COLCHETE; }
"{"             { return ABRE_CHAVE; }
"}"             { return FECHA_CHAVE; }
{numero}        { return NUM; }
{identificador} { return ID; }
\n              { linha++; }
[ \t\r]+        { }
.               { return ERRO; }

%%

char *lexema;
int linha = 1;

void iniciar_scanner(FILE *arquivo)
{
    yyin = arquivo;
}

TipoToken proximo_token(void)
{
    return yylex();
}
