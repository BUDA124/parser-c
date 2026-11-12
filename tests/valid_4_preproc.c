/*
 * tests/valid_4_preproc.c
 * Prueba de preprocesador complejo:
 * Inclusión de cabeceras, macros, comentarios multilínea y de línea.
 * Sin errores sintácticos.
 */

#include "tests/valid_4_helper.h"
#include <stdio.h>

#define MAX_ITEMS 50
#define MULTIPLICADOR 4
#define CODIGO_EXITO 0

/* Comentario de bloque
   que abarca múltiples
   líneas de código fuente
   para verificar la preservación de líneas */

typedef RegistroConfig ConfiguracionLocal;

void config_establecer_defecto(RegistroConfig *cfg) {
    /* Inicializar valores por defecto */
    cfg->id = 100;
    cfg->estado = ESTADO_ACTIVO;
}

int config_validar(const RegistroConfig *cfg) {
    if (cfg->id > 0 && cfg->estado == ESTADO_ACTIVO) {
        return 1;
    }
    return 0;
}

typedef struct Inventario {
    ConfiguracionLocal configuraciones[MAX_ITEMS];
    int cantidad;
    Contador total_items;
} Inventario;

void inventario_init(Inventario *inv) {
    int i;
    inv->cantidad = 0;
    inv->total_items = 0;
    for (i = 0; i < MAX_ITEMS; i = i + 1) {
        config_establecer_defecto(&inv->configuraciones[i]);
    }
}

int inventario_agregar(Inventario *inv, Contador id, int estado) {
    int idx;
    if (inv->cantidad >= MAX_ITEMS) {
        return 0;
    }
    idx = inv->cantidad;
    inv->configuraciones[idx].id = id;
    inv->configuraciones[idx].estado = estado;
    inv->cantidad = inv->cantidad + 1;
    inv->total_items = inv->total_items + (Contador)MULTIPLICADOR;
    return 1;
}

int main(void) {
    Inventario mi_inv;
    int estado_ok;
    Contador x;

    inventario_init(&mi_inv);
    inventario_agregar(&mi_inv, 1, ESTADO_ACTIVO);
    inventario_agregar(&mi_inv, 2, ESTADO_INACTIVO);

    estado_ok = config_validar(&mi_inv.configuraciones[0]);
    if (estado_ok) {
        x = mi_inv.total_items;
    } else {
        x = 0;
    }

    return CODIGO_EXITO;
}
