/*
 * tests/error_4_header.h
 * Cabecera incluida que contiene errores sintácticos intencionales.
 */

typedef struct DatosCabecera {
    int codigo;
    /* Error en cabecera 1: miembro sin tipo */
    campo_invalido;
    float factor;
} DatosCabecera;

/* Error en cabecera 2: prototipo con sintaxis rota */
int procesar_cabecera(int x, + );
