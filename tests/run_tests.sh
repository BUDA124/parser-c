#!/bin/bash
# ============================================================
# tests/run_tests.sh — Script de pruebas automáticas
# ============================================================

set -e

ROJO="\033[1;31m"
VERDE="\033[1;32m"
AZUL="\033[1;34m"
AMARILLO="\033[1;33m"
RESET="\033[0m"

echo -e "${AZUL}============================================================${RESET}"
echo -e "${AZUL}      BATERÍA DE PRUEBAS — PROYECTO 2: PARSER C89/C90      ${RESET}"
echo -e "${AZUL}============================================================${RESET}"

PARSER="./parser"

if [ ! -f "$PARSER" ]; then
    echo -e "${AMARILLO}Compilando el proyecto...${RESET}"
    make
fi

PASSED=0
TOTAL=0

# Función para probar archivos válidos (deben ser completamente mudos y retornar 0)
test_valid() {
    local file=$1
    local name=$2
    TOTAL=$((TOTAL + 1))
    echo -n "Probando válido: $name ($file)... "
    
    output=$( $PARSER "$file" 2>&1 )
    exit_code=$?

    if [ $exit_code -eq 0 ] && [ -z "$output" ]; then
        echo -e "${VERDE}[PASÓ] (Silencio total, código 0)${RESET}"
        PASSED=$((PASSED + 1))
    else
        echo -e "${ROJO}[FALLÓ] (Código: $exit_code, Salida: '$output')${RESET}"
    fi
}

# Función para probar archivos con errores (deben retornar 1 y mostrar errores formateados)
test_error() {
    local file=$1
    local name=$2
    TOTAL=$((TOTAL + 1))
    echo -n "Probando con errores: $name ($file)... "
    
    set +e
    output=$( $PARSER "$file" 2>&1 )
    exit_code=$?
    set -e

    # Debe retornar 1 y contener la palabra "error:" y al menos un puntero "^"
    if [ $exit_code -eq 1 ] && echo "$output" | grep -q "error:" && echo "$output" | grep -q "\^"; then
        count_errors=$(echo "$output" | grep -c "error:" || true)
        echo -e "${VERDE}[PASÓ] (Código 1, detectó $count_errors errores formateados)${RESET}"
        PASSED=$((PASSED + 1))
    else
        echo -e "${ROJO}[FALLÓ] (Código: $exit_code, Salida inesperada)${RESET}"
    fi
}

echo ""
echo -e "${AMARILLO}--- 1. Programas sin errores (Regla de silencio y salida 0) ---${RESET}"
test_valid "tests/valid_1_matrices.c" "Álgebra Lineal (>=500 líneas)"
test_valid "tests/valid_2_datastructs.c" "Estructuras de Datos (>=500 líneas)"
test_valid "tests/valid_3_graph.c" "Algoritmos de Grafos (>=500 líneas)"
test_valid "tests/valid_4_preproc.c" "Preprocesador y Módulos"

echo ""
echo -e "${AMARILLO}--- 2. Programas con múltiples errores sintácticos (Salida 1 y GCC style) ---${RESET}"
test_error "tests/error_1_syntax500.c" "Simulación N-Cuerpos (>=500 líneas)"
test_error "tests/error_2_statements.c" "Sentencias de Control"
test_error "tests/error_3_declarations.c" "Declaraciones y Tipos"
test_error "tests/error_4_includes.c" "Errores en #include cruzados"

echo ""
echo -e "${AMARILLO}--- 3. Verificación de tamaño (al menos 4 programas con >= 500 líneas) ---${RESET}"
c500=0
for f in tests/valid_1_matrices.c tests/valid_2_datastructs.c tests/valid_3_graph.c tests/error_1_syntax500.c; do
    lines=$(wc -l < "$f")
    if [ "$lines" -ge 500 ]; then
        echo -e "  $f: ${VERDE}$lines líneas (>= 500)${RESET}"
        c500=$((c500 + 1))
    else
        echo -e "  $f: ${ROJO}$lines líneas (< 500)${RESET}"
    fi
done

echo ""
echo -e "${AZUL}============================================================${RESET}"
echo -e "Resumen: ${PASSED}/${TOTAL} pruebas pasaron exitosamente."
echo -e "Programas con >= 500 líneas: ${c500}/4 requeridos."
echo -e "${AZUL}============================================================${RESET}"

if [ $PASSED -eq $TOTAL ] && [ $c500 -ge 4 ]; then
    echo -e "${VERDE}¡TODOS LOS REQUISITOS DEL PROYECTO 2 CUMPLIDOS AL 100%!${RESET}"
    exit 0
else
    echo -e "${ROJO}Algunas pruebas no se cumplieron.${RESET}"
    exit 1
fi
