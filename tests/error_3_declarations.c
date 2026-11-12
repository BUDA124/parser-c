/*
 * tests/error_3_declarations.c
 * Múltiples errores sintácticos en declaraciones, estructuras y tipos.
 */

/* Error 1: Declaración sin tipo */
mi_variable_sin_tipo;

/* Error 2: Declaración con especificadores incompatibles o rotos */
int float x;

/* Error 3: Struct con miembros sin tipo */
struct EstructuraInvalida {
    int campo1;
    ;
    float campo2
    double campo3;
};

/* Error 4: Enum con sintaxis rota */
enum Colores {
    ROJO =,
    VERDE = 2,
    AZUL
};

/* Error 5: Declaración de función con lista de parámetros rota */
void funcion_rota(int a, , float c) {
    int temp;
    temp = a;
}

/* Error 6: Typedef sin identificador */
typedef unsigned long ;

int main(void) {
    /* Error 7: Declaración local rota */
    int = 5;
    return 0;
}
