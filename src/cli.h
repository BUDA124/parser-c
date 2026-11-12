#ifndef CLI_H
#define CLI_H

#include <stdio.h>

/* Opciones del programa parseadas desde la línea de comandos */
typedef struct {
    const char *input_path;    /* Ruta al archivo de entrada (obligatorio) */
    int show_help;             /* 1 si se usa -h (mostrar ayuda) */
} ProgramOptions;

/* Parsea los argumentos de la línea de comandos.
 * Retorna 0 en éxito, -1 si hay error de validación. */
int parse_arguments(int argc, char **argv, ProgramOptions *opts);

/* Imprime el mensaje de uso/ayuda */
void print_usage(FILE *out, const char *prog_name);

#endif /* CLI_H */
