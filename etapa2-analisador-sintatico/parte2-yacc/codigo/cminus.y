// gramatica do C-Minus (Louden, apendice A), com _ no lugar de -

%{
#define YYPARSER

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

static TreeNode *savedTree;

static int yylex(void);
static void traceReduce(const char *lhs, const char *rhs);
void yyerror(const char *message);
%}

%error-verbose

// conflito do else pendente
%expect 1

%union {
    const char *name;
    int val;
    struct treeNode *tree;
}

%token ELSE "else" IF "if" INT "int" RETURN "return" VOID "void" WHILE "while"
%token <name> ID "identificador"
%token <val> NUM "numero"
%token PLUS "+" MINUS "-" TIMES "*" OVER "/"
%token LT "<" LE "<=" GT ">" GE ">=" EQ "==" NE "!="
%token ASSIGN "="
%token SEMI ";" COMMA ","
%token LPAREN "(" RPAREN ")"
%token LBRACKET "[" RBRACKET "]"
%token LBRACE "{" RBRACE "}"

%type <tree> program declaration_list declaration var_declaration fun_declaration
%type <tree> params param_list param compound_stmt local_declarations statement_list
%type <tree> statement expression_stmt selection_stmt iteration_stmt return_stmt
%type <tree> expression var simple_expression additive_expression term factor
%type <tree> call args arg_list
%type <name> type_specifier relop addop mulop

%%

program
    : declaration_list
        { savedTree = $1;
          traceReduce("program", "declaration_list"); }
    ;

declaration_list
    : declaration_list declaration
        { $$ = addSibling($1, $2);
          traceReduce("declaration_list", "declaration_list declaration"); }
    | declaration
        { $$ = $1;
          traceReduce("declaration_list", "declaration"); }
    ;

declaration
    : var_declaration
        { $$ = $1;
          traceReduce("declaration", "var_declaration"); }
    | fun_declaration
        { $$ = $1;
          traceReduce("declaration", "fun_declaration"); }
    ;

var_declaration
    : type_specifier ID ";"
        { $$ = newDeclNode(VarK, $1, $2);
          traceReduce("var_declaration", "type_specifier ID ;"); }
    | type_specifier ID "[" NUM "]" ";"
        { $$ = newDeclNode(ArrayK, $1, $2);
          $$->val = $4;
          traceReduce("var_declaration", "type_specifier ID [ NUM ] ;"); }
    ;

type_specifier
    : "int"
        { $$ = "int";
          traceReduce("type_specifier", "int"); }
    | "void"
        { $$ = "void";
          traceReduce("type_specifier", "void"); }
    ;

fun_declaration
    : type_specifier ID "(" params ")" compound_stmt
        { $$ = newDeclNode(FunK, $1, $2);
          $$->child[0] = $4;
          $$->child[1] = $6;
          traceReduce("fun_declaration", "type_specifier ID ( params ) compound_stmt"); }
    ;

params
    : param_list
        { $$ = $1;
          traceReduce("params", "param_list"); }
    | "void"
        { $$ = NULL;
          traceReduce("params", "void"); }
    ;

param_list
    : param_list "," param
        { $$ = addSibling($1, $3);
          traceReduce("param_list", "param_list , param"); }
    | param
        { $$ = $1;
          traceReduce("param_list", "param"); }
    ;

param
    : type_specifier ID
        { $$ = newDeclNode(ParamK, $1, $2);
          traceReduce("param", "type_specifier ID"); }
    | type_specifier ID "[" "]"
        { $$ = newDeclNode(ArrayParamK, $1, $2);
          traceReduce("param", "type_specifier ID [ ]"); }
    ;

compound_stmt
    : "{" local_declarations statement_list "}"
        { $$ = newNode(CompoundK);
          $$->child[0] = $2;
          $$->child[1] = $3;
          traceReduce("compound_stmt", "{ local_declarations statement_list }"); }
    ;

local_declarations
    : local_declarations var_declaration
        { $$ = addSibling($1, $2);
          traceReduce("local_declarations", "local_declarations var_declaration"); }
    | // vazio
        { $$ = NULL;
          traceReduce("local_declarations", "vazio"); }
    ;

statement_list
    : statement_list statement
        { $$ = addSibling($1, $2);
          traceReduce("statement_list", "statement_list statement"); }
    | // vazio
        { $$ = NULL;
          traceReduce("statement_list", "vazio"); }
    ;

statement
    : expression_stmt
        { $$ = $1;
          traceReduce("statement", "expression_stmt"); }
    | compound_stmt
        { $$ = $1;
          traceReduce("statement", "compound_stmt"); }
    | selection_stmt
        { $$ = $1;
          traceReduce("statement", "selection_stmt"); }
    | iteration_stmt
        { $$ = $1;
          traceReduce("statement", "iteration_stmt"); }
    | return_stmt
        { $$ = $1;
          traceReduce("statement", "return_stmt"); }
    ;

expression_stmt
    : expression ";"
        { $$ = $1;
          traceReduce("expression_stmt", "expression ;"); }
    | ";"
        { $$ = newNode(EmptyK);
          traceReduce("expression_stmt", ";"); }
    ;

selection_stmt
    : "if" "(" expression ")" statement
        { $$ = newNode(IfK);
          $$->child[0] = $3;
          $$->child[1] = $5;
          traceReduce("selection_stmt", "if ( expression ) statement"); }
    | "if" "(" expression ")" statement "else" statement
        { $$ = newNode(IfK);
          $$->child[0] = $3;
          $$->child[1] = $5;
          $$->child[2] = $7;
          traceReduce("selection_stmt", "if ( expression ) statement else statement"); }
    ;

iteration_stmt
    : "while" "(" expression ")" statement
        { $$ = newNode(WhileK);
          $$->child[0] = $3;
          $$->child[1] = $5;
          traceReduce("iteration_stmt", "while ( expression ) statement"); }
    ;

return_stmt
    : "return" ";"
        { $$ = newNode(ReturnK);
          traceReduce("return_stmt", "return ;"); }
    | "return" expression ";"
        { $$ = newNode(ReturnK);
          $$->child[0] = $2;
          traceReduce("return_stmt", "return expression ;"); }
    ;

expression
    : var "=" expression
        { $$ = newNode(AssignK);
          $$->child[0] = $1;
          $$->child[1] = $3;
          traceReduce("expression", "var = expression"); }
    | simple_expression
        { $$ = $1;
          traceReduce("expression", "simple_expression"); }
    ;

var
    : ID
        { $$ = newNode(IdK);
          $$->name = $1;
          traceReduce("var", "ID"); }
    | ID "[" expression "]"
        { $$ = newNode(IndexK);
          $$->name = $1;
          $$->child[0] = $3;
          traceReduce("var", "ID [ expression ]"); }
    ;

simple_expression
    : additive_expression relop additive_expression
        { $$ = newOpNode($2, $1, $3);
          traceReduce("simple_expression", "additive_expression relop additive_expression"); }
    | additive_expression
        { $$ = $1;
          traceReduce("simple_expression", "additive_expression"); }
    ;

relop
    : "<="
        { $$ = "<=";
          traceReduce("relop", "<="); }
    | "<"
        { $$ = "<";
          traceReduce("relop", "<"); }
    | ">"
        { $$ = ">";
          traceReduce("relop", ">"); }
    | ">="
        { $$ = ">=";
          traceReduce("relop", ">="); }
    | "=="
        { $$ = "==";
          traceReduce("relop", "=="); }
    | "!="
        { $$ = "!=";
          traceReduce("relop", "!="); }
    ;

additive_expression
    : additive_expression addop term
        { $$ = newOpNode($2, $1, $3);
          traceReduce("additive_expression", "additive_expression addop term"); }
    | term
        { $$ = $1;
          traceReduce("additive_expression", "term"); }
    ;

addop
    : "+"
        { $$ = "+";
          traceReduce("addop", "+"); }
    | "-"
        { $$ = "-";
          traceReduce("addop", "-"); }
    ;

term
    : term mulop factor
        { $$ = newOpNode($2, $1, $3);
          traceReduce("term", "term mulop factor"); }
    | factor
        { $$ = $1;
          traceReduce("term", "factor"); }
    ;

mulop
    : "*"
        { $$ = "*";
          traceReduce("mulop", "*"); }
    | "/"
        { $$ = "/";
          traceReduce("mulop", "/"); }
    ;

factor
    : "(" expression ")"
        { $$ = $2;
          traceReduce("factor", "( expression )"); }
    | var
        { $$ = $1;
          traceReduce("factor", "var"); }
    | call
        { $$ = $1;
          traceReduce("factor", "call"); }
    | NUM
        { $$ = newNode(ConstK);
          $$->val = $1;
          traceReduce("factor", "NUM"); }
    ;

call
    : ID "(" args ")"
        { $$ = newNode(CallK);
          $$->name = $1;
          $$->child[0] = $3;
          traceReduce("call", "ID ( args )"); }
    ;

args
    : arg_list
        { $$ = $1;
          traceReduce("args", "arg_list"); }
    | // vazio
        { $$ = NULL;
          traceReduce("args", "vazio"); }
    ;

arg_list
    : arg_list "," expression
        { $$ = addSibling($1, $3);
          traceReduce("arg_list", "arg_list , expression"); }
    | expression
        { $$ = $1;
          traceReduce("arg_list", "expression"); }
    ;

%%

// quando o Bison pede um token, o anterior ja foi empilhado
static int yylex(void)
{
    TokenType token;

    if (TraceParse)
        printShift();
    token = getToken();
    if (TraceParse)
        printReceive(token);
    return token;
}

// yychar vazio: o token lido ja foi empilhado
static void traceReduce(const char *lhs, const char *rhs)
{
    if (TraceParse)
        printReduce(lhs, rhs, yychar == YYEMPTY);
}

void yyerror(const char *message)
{
    nerrors++;
    printSyntaxError(message);
}

TreeNode *parse(void)
{
    if (yyparse() != 0)
        return NULL;
    return savedTree;
}
