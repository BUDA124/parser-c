/*
 * tests/error_2_statements.c
 * Prueba de múltiples errores sintácticos en sentencias de control
 * (if, while, for, do-while, switch, return) y operadores continuos.
 */

int funcion_con_errores(int a, int b) {
    int x;
    int y;
    int z;

    /* Error 1: Operadores consecutivos inválidos */
    x = a * / b;

    /* Error 2: Condición de if incompleta */
    if (a > ) {
        x = 10;
    } else {
        x = 20;
    }

    /* Error 3: Bucle while sin condición */
    while () {
        x = x - 1;
    }

    /* Error 4: Falta de punto y coma en asignación */
    y = a + b * 2
    z = y / 2;

    /* Error 5: Sentencia do-while sin condición */
    do {
        z = z + 1;
    } while ();

    /* Error 6: Switch con etiqueta case rota */
    switch (x) {
        case :
            y = 100;
            break;
        default:
            y = 200;
            break;
    }

    /* Error 7: Sentencia return con operador colgado */
    return x + ;
}

int main(void) {
    int r;
    r = funcion_con_errores(10, 5);
    return r;
}
