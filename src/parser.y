%{
#define _POSIX_C_SOURCE 200809L
#include "symtab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int yylex(void);
extern char current_filename[];

void yyerror(const char *msg);

/* Contador global de errores encontrados */
int total_errores = 0;

/* Control de ámbito para el Lexer Hack */
static int nivel_parametros = 0;
static int nivel_struct = 0;
%}

%locations
%define parse.error verbose

%union {
    char *sval;
}

%token <sval> IDENTIFIER TYPE_NAME CONSTANT STRING_LITERAL
%token SIZEOF
%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
%token AND_OP OR_OP MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN
%token SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN
%token XOR_ASSIGN OR_ASSIGN

%token TYPEDEF EXTERN STATIC AUTO REGISTER
%token CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE CONST VOLATILE VOID
%token STRUCT UNION ENUM ELLIPSIS

%token CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%start translation_unit

%%

/* ========================================================================= */
/*                              EXPRESIONES                                  */
/* ========================================================================= */

primary_expression
    : IDENTIFIER
        { free($1); }
    | CONSTANT
        { free($1); }
    | STRING_LITERAL
        { free($1); }
    | '(' expression ')'
    ;

postfix_expression
    : primary_expression
    | postfix_expression '[' expression ']'
    | postfix_expression '(' ')'
    | postfix_expression '(' argument_expression_list ')'
    | postfix_expression '.' IDENTIFIER
        { free($3); }
    | postfix_expression '.' TYPE_NAME
        { free($3); }
    | postfix_expression PTR_OP IDENTIFIER
        { free($3); }
    | postfix_expression PTR_OP TYPE_NAME
        { free($3); }
    | postfix_expression INC_OP
    | postfix_expression DEC_OP
    ;

argument_expression_list
    : assignment_expression
    | argument_expression_list ',' assignment_expression
    ;

unary_expression
    : postfix_expression
    | INC_OP unary_expression
    | DEC_OP unary_expression
    | unary_operator cast_expression
    | SIZEOF unary_expression
    | SIZEOF '(' type_name ')'
    ;

unary_operator
    : '&'
    | '*'
    | '+'
    | '-'
    | '~'
    | '!'
    ;

cast_expression
    : unary_expression
    | '(' type_name ')' cast_expression
    ;

multiplicative_expression
    : cast_expression
    | multiplicative_expression '*' cast_expression
    | multiplicative_expression '/' cast_expression
    | multiplicative_expression '%' cast_expression
    ;

additive_expression
    : multiplicative_expression
    | additive_expression '+' multiplicative_expression
    | additive_expression '-' multiplicative_expression
    ;

shift_expression
    : additive_expression
    | shift_expression LEFT_OP additive_expression
    | shift_expression RIGHT_OP additive_expression
    ;

relational_expression
    : shift_expression
    | relational_expression '<' shift_expression
    | relational_expression '>' shift_expression
    | relational_expression LE_OP shift_expression
    | relational_expression GE_OP shift_expression
    ;

equality_expression
    : relational_expression
    | equality_expression EQ_OP relational_expression
    | equality_expression NE_OP relational_expression
    ;

and_expression
    : equality_expression
    | and_expression '&' equality_expression
    ;

exclusive_or_expression
    : and_expression
    | exclusive_or_expression '^' and_expression
    ;

inclusive_or_expression
    : exclusive_or_expression
    | inclusive_or_expression '|' exclusive_or_expression
    ;

logical_and_expression
    : inclusive_or_expression
    | logical_and_expression AND_OP inclusive_or_expression
    ;

logical_or_expression
    : logical_and_expression
    | logical_or_expression OR_OP logical_and_expression
    ;

conditional_expression
    : logical_or_expression
    | logical_or_expression '?' expression ':' conditional_expression
    ;

assignment_expression
    : conditional_expression
    | unary_expression assignment_operator assignment_expression
    ;

assignment_operator
    : '='
    | MUL_ASSIGN
    | DIV_ASSIGN
    | MOD_ASSIGN
    | ADD_ASSIGN
    | SUB_ASSIGN
    | LEFT_ASSIGN
    | RIGHT_ASSIGN
    | AND_ASSIGN
    | XOR_ASSIGN
    | OR_ASSIGN
    ;

expression
    : assignment_expression
    | expression ',' assignment_expression
    ;

constant_expression
    : conditional_expression
    ;

/* ========================================================================= */
/*                             DECLARACIONES                                 */
/* ========================================================================= */

declaration
    : declaration_specifiers ';'
        { en_declaracion_typedef = 0; }
    | declaration_specifiers init_declarator_list ';'
        { en_declaracion_typedef = 0; }
    ;

declaration_specifiers
    : storage_class_specifier
    | storage_class_specifier declaration_specifiers
    | type_specifier
    | type_specifier declaration_specifiers
    | type_qualifier
    | type_qualifier declaration_specifiers
    ;

init_declarator_list
    : init_declarator
    | init_declarator_list ',' init_declarator
    ;

init_declarator
    : declarator
    | declarator '=' initializer
    ;

storage_class_specifier
    : TYPEDEF
        { en_declaracion_typedef = 1; }
    | EXTERN
    | STATIC
    | AUTO
    | REGISTER
    ;

type_specifier
    : VOID
    | CHAR
    | SHORT
    | INT
    | LONG
    | FLOAT
    | DOUBLE
    | SIGNED
    | UNSIGNED
    | struct_or_union_specifier
    | enum_specifier
    | TYPE_NAME
        { free($1); }
    ;

struct_or_union_specifier
    : struct_or_union IDENTIFIER '{' enter_struct struct_declaration_list '}' leave_struct
        { free($2); }
    | struct_or_union '{' enter_struct struct_declaration_list '}' leave_struct
    | struct_or_union IDENTIFIER
        { free($2); }
    ;

enter_struct
    : /* vacío */
        { nivel_struct++; }
    ;

leave_struct
    : /* vacío */
        { nivel_struct--; }
    ;

struct_or_union
    : STRUCT
    | UNION
    ;

struct_declaration_list
    : struct_declaration
    | struct_declaration_list struct_declaration
    ;

struct_declaration
    : specifier_qualifier_list struct_declarator_list ';'
    ;

specifier_qualifier_list
    : type_specifier specifier_qualifier_list
    | type_specifier
    | type_qualifier specifier_qualifier_list
    | type_qualifier
    ;

struct_declarator_list
    : struct_declarator
    | struct_declarator_list ',' struct_declarator
    ;

struct_declarator
    : declarator
    | ':' constant_expression
    | declarator ':' constant_expression
    ;

enum_specifier
    : ENUM '{' enumerator_list '}'
    | ENUM IDENTIFIER '{' enumerator_list '}'
        { free($2); }
    | ENUM IDENTIFIER
        { free($2); }
    ;

enumerator_list
    : enumerator
    | enumerator_list ',' enumerator
    ;

enumerator
    : IDENTIFIER
        { free($1); }
    | IDENTIFIER '=' constant_expression
        { free($1); }
    ;

type_qualifier
    : CONST
    | VOLATILE
    ;

declarator
    : pointer direct_declarator
    | direct_declarator
    ;

direct_declarator
    : IDENTIFIER
        {
            if (en_declaracion_typedef && nivel_struct == 0 && nivel_parametros == 0) {
                registrar_typedef($1);
            }
            free($1);
        }
    | '(' declarator ')'
    | direct_declarator '[' constant_expression ']'
    | direct_declarator '[' ']'
    | direct_declarator '(' parameter_type_list ')'
    | direct_declarator '(' identifier_list ')'
    | direct_declarator '(' ')'
    ;

pointer
    : '*'
    | '*' type_qualifier_list
    | '*' pointer
    | '*' type_qualifier_list pointer
    ;

type_qualifier_list
    : type_qualifier
    | type_qualifier_list type_qualifier
    ;

enter_params
    : /* vacío */
        { nivel_parametros++; }
    ;

leave_params
    : /* vacío */
        { nivel_parametros--; }
    ;

parameter_type_list
    : enter_params parameter_list leave_params
    | enter_params parameter_list ',' ELLIPSIS leave_params
    ;

parameter_list
    : parameter_declaration
    | parameter_list ',' parameter_declaration
    ;

parameter_declaration
    : declaration_specifiers declarator
    | declaration_specifiers abstract_declarator
    | declaration_specifiers
    ;

identifier_list
    : IDENTIFIER
        { free($1); }
    | identifier_list ',' IDENTIFIER
        { free($3); }
    ;

type_name
    : specifier_qualifier_list
    | specifier_qualifier_list abstract_declarator
    ;

abstract_declarator
    : pointer
    | direct_abstract_declarator
    | pointer direct_abstract_declarator
    ;

direct_abstract_declarator
    : '(' abstract_declarator ')'
    | '[' ']'
    | '[' constant_expression ']'
    | direct_abstract_declarator '[' ']'
    | direct_abstract_declarator '[' constant_expression ']'
    | '(' ')'
    | '(' parameter_type_list ')'
    | direct_abstract_declarator '(' ')'
    | direct_abstract_declarator '(' parameter_type_list ')'
    ;

initializer
    : assignment_expression
    | '{' initializer_list '}'
    | '{' initializer_list ',' '}'
    ;

initializer_list
    : initializer
    | initializer_list ',' initializer
    ;

/* ========================================================================= */
/*                               SENTENCIAS                                  */
/* ========================================================================= */

statement
    : labeled_statement
    | compound_statement
    | expression_statement
    | selection_statement
    | iteration_statement
    | jump_statement
    | error ';'
        { yyerrok; }
    ;

labeled_statement
    : IDENTIFIER ':' statement
        { free($1); }
    | CASE constant_expression ':' statement
    | DEFAULT ':' statement
    ;

compound_statement
    : '{' '}'
    | '{' statement_list '}'
    | '{' declaration_list '}'
    | '{' declaration_list statement_list '}'
    | '{' error '}'
        { yyerrok; }
    ;

declaration_list
    : declaration
    | declaration_list declaration
    ;

statement_list
    : statement
    | statement_list statement
    ;

expression_statement
    : ';'
    | expression ';'
    ;

selection_statement
    : IF '(' expression ')' statement %prec LOWER_THAN_ELSE
    | IF '(' expression ')' statement ELSE statement
    | SWITCH '(' expression ')' statement
    ;

iteration_statement
    : WHILE '(' expression ')' statement
    | DO statement WHILE '(' expression ')' ';'
    | FOR '(' expression_statement expression_statement ')' statement
    | FOR '(' expression_statement expression_statement expression ')' statement
    ;

jump_statement
    : GOTO IDENTIFIER ';'
        { free($2); }
    | CONTINUE ';'
    | BREAK ';'
    | RETURN ';'
    | RETURN expression ';'
    ;

/* ========================================================================= */
/*                        UNIDAD DE TRADUCCIÓN (RAÍZ)                        */
/* ========================================================================= */

translation_unit
    : /* vacío */
    | translation_unit external_declaration
    ;

external_declaration
    : function_definition
    | declaration
    | error ';'
        { en_declaracion_typedef = 0; yyerrok; }
    ;

function_definition
    : declaration_specifiers declarator declaration_list compound_statement
    | declaration_specifiers declarator compound_statement
    | declarator declaration_list compound_statement
    | declarator compound_statement
    ;

%%

void yyerror(const char *msg) {
    total_errores++;

    const char *fname = current_filename[0] ? current_filename : "<desconocido>";
    int line = yylloc.first_line > 0 ? yylloc.first_line : 1;
    int col = yylloc.first_column > 0 ? yylloc.first_column : 1;

    /* 1. Encabezado del error con formato estilo GCC y colores ANSI */
    fprintf(stderr, "\033[1m%s:%d:%d:\033[0m \033[1;31merror:\033[0m %s\n",
            fname, line, col, msg);

    /* 2. Extracción de la línea exacta del archivo fuente original */
    FILE *src = fopen(fname, "r");
    if (!src) {
        return;
    }

    char buffer_linea[8192];
    int current_line_num = 0;
    int found = 0;
    char last_read_line[8192];
    last_read_line[0] = '\0';

    while (fgets(buffer_linea, sizeof(buffer_linea), src) != NULL) {
        current_line_num++;
        strncpy(last_read_line, buffer_linea, sizeof(last_read_line) - 1);
        last_read_line[sizeof(last_read_line) - 1] = '\0';
        if (current_line_num == line) {
            found = 1;
            break;
        }
    }
    fclose(src);

    /* Si se llegó a EOF antes de la línea (e.g. error en fin de archivo), usar la última leída */
    if (!found && last_read_line[0] != '\0') {
        strncpy(buffer_linea, last_read_line, sizeof(buffer_linea) - 1);
        buffer_linea[sizeof(buffer_linea) - 1] = '\0';
        found = 1;
    }

    if (found) {
        /* Asegurar terminación en \n */
        size_t len = strlen(buffer_linea);
        if (len > 0 && buffer_linea[len - 1] == '\r') {
            buffer_linea[len - 1] = '\0';
            len--;
        }
        if (len == 0 || buffer_linea[len - 1] != '\n') {
            if (len < sizeof(buffer_linea) - 2) {
                buffer_linea[len] = '\n';
                buffer_linea[len + 1] = '\0';
                len++;
            }
        }

        /* Imprimir la línea numerada con margen vertical */
        fprintf(stderr, " %4d | %s", line, buffer_linea);

        /* 3. Indicador visual exacto (^) alineado con tabulaciones */
        fprintf(stderr, "      | ");
        int limit = col - 1;
        for (int i = 0; i < limit; i++) {
            if (i < (int)len && buffer_linea[i] == '\t') {
                fputc('\t', stderr);
            } else {
                fputc(' ', stderr);
            }
        }
        fprintf(stderr, "\033[1;32m^\033[0m\n");
    }
}
