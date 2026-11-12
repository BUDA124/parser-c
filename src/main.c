#include "cli.h"
#include "preprocessor.h"
#include "symtab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Declaraciones externas del parser/lexer generados por Bison/Flex */
extern FILE *yyin;
extern int yyparse(void);
extern char current_filename[];
extern int yylineno;
extern int yycolumn;
extern int total_errores;

int main(int argc, char **argv) {
    ProgramOptions options;
    memset(&options, 0, sizeof(options));

    /* 1. Validar argumentos */
    if (parse_arguments(argc, argv, &options) != 0) {
        return EXIT_FAILURE;
    }
    if (options.show_help) {
        print_usage(stdout, argv[0]);
        return EXIT_SUCCESS;
    }

    /* 2. Verificar que el archivo de entrada se pueda abrir */
    FILE *test_file = fopen(options.input_path, "r");
    if (!test_file) {
        fprintf(stderr, "Error: No se pudo abrir el archivo '%s'\n", options.input_path);
        return EXIT_FAILURE;
    }
    fclose(test_file);

    /* 3. Construir ruta del archivo temporal preprocesado */
    char temp_path[512];
    snprintf(temp_path, sizeof(temp_path), "%s.pre", options.input_path);

    /* 4. Preprocesar */
    if (preprocess_file(options.input_path, temp_path) != 0) {
        fprintf(stderr, "Error en preprocesamiento: %s\n",
                preprocessor_last_error() ? preprocessor_last_error() : "desconocido");
        return EXIT_FAILURE;
    }

    /* 5. Abrir archivo temporal para el parser */
    yyin = fopen(temp_path, "r");
    if (!yyin) {
        fprintf(stderr, "Error: No se pudo abrir el archivo preprocesado '%s'\n", temp_path);
        return EXIT_FAILURE;
    }

    /* 6. Inicializar estado del analizador */
    strncpy(current_filename, options.input_path, 511);
    current_filename[511] = '\0';
    yylineno = 1;
    yycolumn = 1;
    symtab_init();

    /* 7. Ejecutar el parser (Bison invoca yylex de Flex internamente) */
    int parse_result = yyparse();

    fclose(yyin);
    symtab_destroy();

    /* 8. Regla de silencio: si no hay errores, salida muda.
     *    Si hay errores, yyerror ya los imprimió en stderr. */
    return (parse_result == 0 && total_errores == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
