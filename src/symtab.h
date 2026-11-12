#ifndef SYMTAB_H
#define SYMTAB_H

/* Bandera global: 1 si estamos procesando una declaración typedef */
extern int en_declaracion_typedef;

/* Inicializa la tabla de símbolos y precarga tipos estándar */
void symtab_init(void);

/* Verifica si un identificador fue registrado previamente como un tipo */
int es_typedef(const char *name);

/* Registra un nuevo identificador en la tabla de typedefs */
void registrar_typedef(const char *name);

/* Libera la memoria utilizada por la tabla de símbolos */
void symtab_destroy(void);

#endif /* SYMTAB_H */
