%{
#define _POSIX_C_SOURCE 200809L
#include "parser.tab.h"
#include "symtab.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char current_filename[512] = "";
int yycolumn = 1;

void yyerror(const char *msg);

#define YY_USER_ACTION \
    yylloc.first_line = yylloc.last_line = yylineno; \
    yylloc.first_column = yycolumn; \
    yylloc.last_column = yycolumn + yyleng - 1; \
    yycolumn += yyleng;
%}

%option yylineno
%option noyywrap
%option nounput
%option noinput

/* Expresiones regulares para componentes léxicos */
O   [0-7]
D   [0-9]
NZ  [1-9]
L   [a-zA-Z_]
A   [a-zA-Z_0-9]
H   [a-fA-F0-9]
HP  (0[xX])
E   ([Ee][+-]?{D}+)
P   ([Pp][+-]?{D}+)
FS  (f|F|l|L)
IS  (((u|U)(l|L|ll|LL)?)|((l|L|ll|LL)(u|U)?))

%%

 /* ================= 1. Directivas #line inyectadas por el preprocesador ================= */
^[ \t]*"#line"[ \t]+{D}+[ \t]+\"[^\"]+\"[ \t]*\r?\n {
    int nueva_linea;
    char nuevo_archivo[512];
    char *p = strstr(yytext, "#line");
    if (p && sscanf(p, "#line %d \"%511[^\"]\"", &nueva_linea, nuevo_archivo) == 2) {
        yylineno = nueva_linea;
        strncpy(current_filename, nuevo_archivo, sizeof(current_filename) - 1);
        current_filename[sizeof(current_filename) - 1] = '\0';
    }
    yycolumn = 1;
}

 /* ================= 2. Espacios en blanco y saltos de línea ================= */
\n {
    yycolumn = 1;
}

[ \t\v\f\r]+ {
    /* Ignorar espacios en blanco; YY_USER_ACTION ya avanzó yycolumn */
}

 /* ================= 3. Comentarios de respaldo (fallback por seguridad) ================= */
"/*"([^*]|\*+[^*/])*\*+"/" {
    for (int i = 0; i < yyleng; i++) {
        if (yytext[i] == '\n') {
            yycolumn = 1;
        }
    }
}
"//"[^\n]* {
    /* Comentario de línea */
}

 /* ================= 4. Palabras reservadas (ANSI C C89/C90) ================= */
"auto"                  { return AUTO; }
"break"                 { return BREAK; }
"case"                  { return CASE; }
"char"                  { return CHAR; }
"const"                 { return CONST; }
"continue"              { return CONTINUE; }
"default"               { return DEFAULT; }
"do"                    { return DO; }
"double"                { return DOUBLE; }
"else"                  { return ELSE; }
"enum"                  { return ENUM; }
"extern"                { return EXTERN; }
"float"                 { return FLOAT; }
"for"                   { return FOR; }
"goto"                  { return GOTO; }
"if"                    { return IF; }
"int"                   { return INT; }
"long"                  { return LONG; }
"register"              { return REGISTER; }
"return"                { return RETURN; }
"short"                 { return SHORT; }
"signed"                { return SIGNED; }
"sizeof"                { return SIZEOF; }
"static"                { return STATIC; }
"struct"                { return STRUCT; }
"switch"                { return SWITCH; }
"typedef"               { return TYPEDEF; }
"union"                 { return UNION; }
"unsigned"              { return UNSIGNED; }
"void"                  { return VOID; }
"volatile"              { return VOLATILE; }
"while"                 { return WHILE; }

 /* ================= 5. Identificadores y Lexer Hack ================= */
{L}{A}* {
    yylval.sval = strdup(yytext);
    if (es_typedef(yytext)) {
        return TYPE_NAME;
    }
    return IDENTIFIER;
}

 /* ================= 6. Constantes (enteras, flotantes, de carácter) ================= */
{HP}{H}+{IS}?           { yylval.sval = strdup(yytext); return CONSTANT; }
0{O}*{IS}?              { yylval.sval = strdup(yytext); return CONSTANT; }
{NZ}{D}*{IS}?           { yylval.sval = strdup(yytext); return CONSTANT; }

{D}+{E}{FS}?            { yylval.sval = strdup(yytext); return CONSTANT; }
{D}*"."{D}+{E}?{FS}?    { yylval.sval = strdup(yytext); return CONSTANT; }
{D}+"."{D}*{E}?{FS}?    { yylval.sval = strdup(yytext); return CONSTANT; }
{HP}{H}+{P}{FS}?        { yylval.sval = strdup(yytext); return CONSTANT; }
{HP}{H}*"."{H}+{P}{FS}? { yylval.sval = strdup(yytext); return CONSTANT; }
{HP}{H}+"."{H}*{P}{FS}? { yylval.sval = strdup(yytext); return CONSTANT; }

L?'(\\.|[^\\'\n])+'     { yylval.sval = strdup(yytext); return CONSTANT; }

 /* ================= 7. Literales de cadena ================= */
L?\"(\\.|[^\\"\n])*\"   { yylval.sval = strdup(yytext); return STRING_LITERAL; }

 /* Literales sin terminar (reporte de error léxico) */
L?\"(\\.|[^\\"\n])*     {
    yyerror("cadena de texto no terminada");
    yylval.sval = strdup(yytext);
    return STRING_LITERAL;
}
L?'(\\.|[^\\'\n])*      {
    yyerror("literal de caracter no terminado");
    yylval.sval = strdup(yytext);
    return CONSTANT;
}

 /* ================= 8. Operadores compuestos ================= */
"..."                   { return ELLIPSIS; }
">>="                   { return RIGHT_ASSIGN; }
"<<="                   { return LEFT_ASSIGN; }
"+="                    { return ADD_ASSIGN; }
"-="                    { return SUB_ASSIGN; }
"*="                    { return MUL_ASSIGN; }
"/="                    { return DIV_ASSIGN; }
"%="                    { return MOD_ASSIGN; }
"&="                    { return AND_ASSIGN; }
"^="                    { return XOR_ASSIGN; }
"|="                    { return OR_ASSIGN; }
">>"                    { return RIGHT_OP; }
"<<"                    { return LEFT_OP; }
"++"                    { return INC_OP; }
"--"                    { return DEC_OP; }
"->"                    { return PTR_OP; }
"&&"                    { return AND_OP; }
"||"                    { return OR_OP; }
"<="                    { return LE_OP; }
">="                    { return GE_OP; }
"=="                    { return EQ_OP; }
"!="                    { return NE_OP; }

 /* ================= 9. Caracteres individuales y digrafos ================= */
";"                     { return ';'; }
("{"|"<%")              { return '{'; }
("}"|"%>")              { return '}'; }
","                     { return ','; }
":"                     { return ':'; }
"="                     { return '='; }
"("                     { return '('; }
")"                     { return ')'; }
("["|"<:")              { return '['; }
("]"|":>")              { return ']'; }
"."                     { return '.'; }
"&"                     { return '&'; }
"!"                     { return '!'; }
"~"                     { return '~'; }
"-"                     { return '-'; }
"+"                     { return '+'; }
"*"                     { return '*'; }
"/"                     { return '/'; }
"%"                     { return '%'; }
"<"                     { return '<'; }
">"                     { return '>'; }
"^"                     { return '^'; }
"|"                     { return '|'; }
"?"                     { return '?'; }

 /* ================= 10. Errores léxicos / Caracteres no reconocidos ================= */
.                       { return yytext[0]; }

<<EOF>>                 { return 0; }

%%
