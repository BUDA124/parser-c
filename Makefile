# ============================================================
# Makefile — Proyecto 2: Analizador Sintáctico (C / Flex / Bison)
# ============================================================

CC       := gcc
CFLAGS   := -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -g -Isrc -Ibuild
LDFLAGS  :=

SRC_DIR   := src
BUILD_DIR := build

TARGET   := parser

# Objetos (en build/)
OBJS := $(BUILD_DIR)/main.o         \
        $(BUILD_DIR)/cli.o          \
        $(BUILD_DIR)/util.o         \
        $(BUILD_DIR)/symtab.o       \
        $(BUILD_DIR)/preprocessor.o \
        $(BUILD_DIR)/parser.tab.o   \
        $(BUILD_DIR)/lex.yy.o

# ============================================================
# Regla principal
# ============================================================

.PHONY: all clean test package

all: $(TARGET)

test: $(TARGET)
	bash tests/run_tests.sh

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# ============================================================
# Bison: parser.y -> build/parser.tab.c + build/parser.tab.h
# ============================================================

$(BUILD_DIR)/parser.tab.c $(BUILD_DIR)/parser.tab.h: $(SRC_DIR)/parser.y | $(BUILD_DIR)
	bison -d -v -o $(BUILD_DIR)/parser.tab.c $<

$(BUILD_DIR)/parser.tab.o: $(BUILD_DIR)/parser.tab.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -Isrc -w -c -o $@ $<

# ============================================================
# Flex: lexer.l -> build/lex.yy.c
# ============================================================

$(BUILD_DIR)/lex.yy.c: $(SRC_DIR)/lexer.l $(BUILD_DIR)/parser.tab.h | $(BUILD_DIR)
	flex -o $@ $<

$(BUILD_DIR)/lex.yy.o: $(BUILD_DIR)/lex.yy.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -Isrc -w -c -o $@ $<

# ============================================================
# Archivos .c normales -> build/*.o
# ============================================================

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

# ============================================================
# Creación de directorios
# ============================================================

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# ============================================================
# Limpieza
# ============================================================

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(TARGET)
	rm -f tests/*.pre *.pre
	rm -f proyecto2_grupo.tgz

# ============================================================
# Empaquetado
# ============================================================

package: clean
	tar -czvf proyecto2_grupo.tgz \
		$(SRC_DIR)/ \
		tests/ \
		Makefile \
		README.md
