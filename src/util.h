#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>

/* Crea el directorio si no existe. Retorna 0 en éxito, -1 en error. */
int ensure_directory_exists(const char *dir_path);

#endif /* UTIL_H */
