/*
 * tests/valid_4_helper.h
 * Cabecera auxiliar para probar preprocesador y tipado ANSI C.
 */

#define TAM_BUFFER 128
#define ESTADO_ACTIVO 1
#define ESTADO_INACTIVO 0

typedef unsigned int Contador;
typedef char Nombre[64];

typedef struct RegistroConfig {
    Contador id;
    int estado;
    Nombre etiqueta;
} RegistroConfig;

void config_establecer_defecto(RegistroConfig *cfg);
int config_validar(const RegistroConfig *cfg);
