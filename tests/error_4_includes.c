/*
 * tests/error_4_includes.c
 * Prueba de reporte de errores a través de múltiples archivos incluidos con #include.
 * Los errores deben ser reportados con el nombre de archivo y línea correspondiente
 * a cada archivo (tanto la cabecera como el archivo principal).
 */

#include "tests/error_4_header.h"

int funcion_principal(void) {
    int valor;
    /* Error en archivo principal 1: operadores consecutivos inválidos */
    valor = 10 * / 2;

    /* Error en archivo principal 2: sentencia if rota */
    if (valor < ) {
        valor = 0;
    }

    return valor;
}

int main(void) {
    int res;
    res = funcion_principal();
    return res;
}
