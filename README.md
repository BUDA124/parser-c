# Proyecto 2: Analizador Sintáctico (C / Flex / Bison)

Analizador sintáctico para el lenguaje ANSI C (C89/C90) construido en C puro sobre Linux nativo, utilizando Flex para el escáner léxico y Bison para el autómata de pila LALR(1) con **exactamente 0 conflictos**.

---

## 1. Arquitectura del Flujo de Datos

El analizador funciona como una tubería en cinco etapas:

```
[Archivo Fuente] 
       │
       ▼
 [Preprocesador]  ──> Elimina comentarios, expande #include / #define,
       │               inyecta directivas #line para rastreo exacto.
       ▼
[Archivo Temporal (<archivo>.pre)]
       │
       ▼
    [Scanner]     ──> Flex: servidores de tokens para Bison (yylex),
       │               consulta tabla de símbolos (Lexer Hack).
       ▼
    [Parser]      ──> Bison LALR(1): 0 conflictos, validación sintáctica
       │               y alimentación de la tabla de typedefs.
       ▼
[Salida Consola]  ──> Silencio absoluto si no hay errores (código 0).
                       Formato visual estilo GCC con colores ANSI (código 1).
```

---

## 2. Requisitos Previos

- Linux nativo (Ubuntu / Debian / Fedora / Arch)
- `gcc` (>= 9) con soporte C11
- `bison` (>= 3.5)
- `flex` (>= 2.6)
- `make`

Instalación en Ubuntu/Debian:
```bash
sudo apt update
sudo apt install build-essential bison flex
```

---

## 3. Compilación y Ejecución

- **Compilar:**
  ```bash
  make
  ```
  Genera el binario ejecutable `./parser`.

- **Limpiar artefactos de compilación:**
  ```bash
  make clean
  ```

- **Ejecutar la batería de pruebas:**
  ```bash
  make test
  ```

- **Empaquetar para entrega:**
  ```bash
  make package
  ```
  Genera el archivo comprimido `proyecto2_grupo.tgz`.

---

## 4. Uso de la Interfaz de Línea de Comandos (CLI)

```bash
./parser [opciones] <archivo_entrada>
```

### Opciones
- `-h`: Muestra el menú de ayuda y sale limpiamente con código 0.

### Reglas de Salida
1. **Regla de Oro de Silencio:** Si el archivo fuente no contiene errores léxicos ni sintácticos, el programa termina en **silencio absoluto** (cero texto en `stdout` ni `stderr`) y retorna `0`.
2. **Reporte estilo GCC:** Si se detectan errores sintácticos o léxicos, el programa imprime cada error con el nombre de archivo, línea, columna, mensaje, la línea original y el indicador visual `^` verde alineado con tabulaciones, retornando `1`.
3. **Validación:** Si se ejecuta sin argumentos, con banderas inválidas o con más archivos de los esperados, muestra la ayuda y sale con código `1`.

### Ejemplos
```bash
./parser tests/valid_1_matrices.c    # Silencio total, código de retorno 0
./parser tests/error_1_syntax500.c   # Reporta errores formateados, código 1
./parser -h                          # Muestra la ayuda
```

---

## 5. Características Destacadas de la Implementación

### Cero Conflictos en Bison
La gramática estratificada de ANSI C89 resuelve el clásico conflicto *dangling else* mediante precedencia no asociativa artificial:
```bison
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
...
selection_statement
    : IF '(' expression ')' statement %prec LOWER_THAN_ELSE
    | IF '(' expression ')' statement ELSE statement
    | SWITCH '(' expression ')' statement
    ;
```
Resultado verificado: **0 conflictos Shift/Reduce y 0 conflictos Reduce/Reduce**.

### Lexer Hack (`typedef`)
El lexer distingue si un identificador es `TYPE_NAME` o `IDENTIFIER` consultando `es_typedef()`. La gramática alimenta la tabla de símbolos al reducir `TYPEDEF` en `storage_class_specifier`, con control de ámbito para excluir parámetros de funciones y miembros de estructuras (`struct`/`union`). Se precargan tipos estándar de la biblioteca C (`size_t`, `FILE`, `bool`, etc.).

### Rastreabilidad con `#line`
El preprocesador inyecta marcas de sincronización:
- Al inicio del archivo principal
- Antes y después de cada `#include`
- Preserva los saltos de línea `\n` en comentarios multilínea

El escáner atrapa estas directivas silenciosamente y actualiza `current_filename` y `yylineno`, permitiendo que errores dentro de archivos incluidos apunten con exactitud milimétrica al archivo y línea real.

### Recuperación en Modo Pánico
Se implementan puntos de sincronización con `error ';'` en sentencias y declaraciones externas, y `{ error }` en bloques compuestos, permitiendo encontrar múltiples errores en una sola ejecución sin caer en cascadas infinitas.

---

## 6. Batería de Pruebas (`tests/`)

El proyecto incluye 8 programas en C creados especialmente para la validación:

### 4 Programas Válidos (Silencio absoluto, código 0):
1. `tests/valid_1_matrices.c` (**521 líneas**): Álgebra lineal, inversión Gauss-Jordan, descomposición LU, sistemas lineales.
2. `tests/valid_2_datastructs.c` (**585 líneas**): Árbol AVL balanceado, lista doblemente enlazada, cola de prioridad (heap), tabla hash.
3. `tests/valid_3_graph.c` (**515 líneas**): Grafos con matriz y lista, BFS, DFS, Dijkstra, Prim, Kruskal con DSU, orden topológico.
4. `tests/valid_4_preproc.c` y `tests/valid_4_helper.h`: Preprocesador modular, macros, includes y tipado.

### 4 Programas con Múltiples Errores (Formato GCC, código 1):
5. `tests/error_1_syntax500.c` (**518 líneas**): Simulación física extensa con operadores seguidos, paréntesis desbalanceados, comas colgantes, etc.
6. `tests/error_2_statements.c`: Errores sintácticos en sentencias de control (`if`, `while`, `for`, `switch`, `return`).
7. `tests/error_3_declarations.c`: Errores en declaraciones, structs incompletos, enums y prototipos rotos.
8. `tests/error_4_includes.c` y `tests/error_4_header.h`: Errores sintácticos dentro de cabeceras incluidas y archivo principal.

**Requisito de tamaño:** 4 programas cuentan con $\ge 500$ líneas de código reales.
