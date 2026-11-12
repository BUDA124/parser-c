#define _POSIX_C_SOURCE 200809L
#include "symtab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TYPES 4096

static char *tabla_typedefs[MAX_TYPES];
static int total_typedefs = 0;

int en_declaracion_typedef = 0;

/* Tipos predeterminados del estándar C y librerías comunes */
static const char *tipos_precargados[] = {
    "size_t", "ssize_t", "ptrdiff_t", "intptr_t", "uintptr_t",
    "FILE", "fpos_t", "va_list", "jmp_buf", "time_t", "clock_t",
    "int8_t", "int16_t", "int32_t", "int64_t",
    "uint8_t", "uint16_t", "uint32_t", "uint64_t",
    "int_least8_t", "int_least16_t", "int_least32_t", "int_least64_t",
    "uint_least8_t", "uint_least16_t", "uint_least32_t", "uint_least64_t",
    "int_fast8_t", "int_fast16_t", "int_fast32_t", "int_fast64_t",
    "uint_fast8_t", "uint_fast16_t", "uint_fast32_t", "uint_fast64_t",
    "intmax_t", "uintmax_t",
    "bool", "_Bool",
    "wchar_t", "wint_t",
    NULL
};

void symtab_init(void) {
    symtab_destroy();
    for (int i = 0; tipos_precargados[i] != NULL; i++) {
        registrar_typedef(tipos_precargados[i]);
    }
}

int es_typedef(const char *name) {
    if (!name) return 0;
    for (int i = 0; i < total_typedefs; i++) {
        if (strcmp(tabla_typedefs[i], name) == 0) {
            return 1;
        }
    }
    return 0;
}

void registrar_typedef(const char *name) {
    if (!name) return;
    if (es_typedef(name)) return;
    if (total_typedefs < MAX_TYPES) {
        tabla_typedefs[total_typedefs++] = strdup(name);
    }
}

void symtab_destroy(void) {
    for (int i = 0; i < total_typedefs; i++) {
        free(tabla_typedefs[i]);
        tabla_typedefs[i] = NULL;
    }
    total_typedefs = 0;
    en_declaracion_typedef = 0;
}
