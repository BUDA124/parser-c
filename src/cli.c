#define _POSIX_C_SOURCE 200809L
#include "cli.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void print_usage(FILE *stream, const char *prog_name) {
    fprintf(stream,
        "Uso: %s [opciones] <archivo_entrada>\n"
        "\n"
        "Analizador sintáctico de C (C89/C90).\n"
        "Preprocesa, escanea y valida la sintaxis del archivo fuente.\n"
        "Si no hay errores, el programa termina en silencio absoluto.\n"
        "\n"
        "Opciones:\n"
        "  -h          Mostrar este mensaje de ayuda\n"
        "\n"
        "Ejemplos:\n"
        "  %s programa.c\n"
        "  %s archivo.txt\n"
        "  %s -h\n"
        "\n"
        "El archivo de entrada puede tener cualquier extensión.\n"
        "El archivo temporal preprocesado se genera como <archivo>.pre\n"
        "\n"
        "Códigos de retorno:\n"
        "  0   Ejecución exitosa (sin errores sintácticos)\n"
        "  1   Se encontraron errores sintácticos o léxicos\n",
        prog_name, prog_name, prog_name, prog_name);
}

int parse_arguments(int argc, char **argv, ProgramOptions *options) {
    int opt;

    memset(options, 0, sizeof(*options));

    optind = 1; /* Reset getopt */

    while ((opt = getopt(argc, argv, "h")) != -1) {
        switch (opt) {
            case 'h':
                options->show_help = 1;
                break;
            default:
                /* Bandera desconocida: mostrar ayuda */
                print_usage(stderr, argv[0]);
                return -1;
        }
    }

    if (options->show_help) {
        return 0;
    }

    if (optind >= argc) {
        /* Sin archivo de entrada: mostrar ayuda */
        print_usage(stderr, argv[0]);
        return -1;
    }

    if (optind + 1 < argc) {
        /* Más archivos de los esperados: mostrar ayuda */
        print_usage(stderr, argv[0]);
        return -1;
    }

    options->input_path = argv[optind];

    return 0;
}
