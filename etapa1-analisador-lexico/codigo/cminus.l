%{
#include <string.h>
#include "scanner.h"
%}

%option noyywrap nounput

digito        [0-9]
numero        {digito}+
letra         [a-zA-Z]
identificador {letra}+

%%

"/*"            { int c, anterior = 0, quebras = 0;
                  for (;;) {
                      /* o input() do flex devolve 0 no fim do arquivo, nao EOF */
                      c = input();
                      if (c == 0)
                          return ERRO;
                      if (c == '\n')
                          quebras++;
                      /* soma as quebras so no fim: comentario aberto acusa erro na linha em que abriu */
                      if (anterior == '*' && c == '/') {
                          linha += quebras;
                          break;
                      }
                      anterior = c;
                  }
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

char lexema[TAMANHO_MAXIMO_LEXEMA + 1];
int linha = 1;

void iniciar_scanner(FILE *arquivo)
{
    yyin = arquivo;
}

TipoToken proximo_token(void)
{
    TipoToken token = yylex();

    /* o fim de arquivo dentro de um comentario apaga o yytext */
    if (token == ERRO && yytext[0] == '\0') {
        strcpy(lexema, "/*");
        return token;
    }

    strncpy(lexema, yytext, TAMANHO_MAXIMO_LEXEMA);
    lexema[TAMANHO_MAXIMO_LEXEMA] = '\0';
    return token;
}
