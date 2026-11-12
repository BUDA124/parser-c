/*
 * tests/valid_1_matrices.c
 * Biblioteca de álgebra lineal y cálculo matricial en ANSI C (C89).
 * Sin errores sintácticos.
 */

#define MAX_DIM 32
#define EPSILON 0.000001

typedef double Real;
typedef int Indice;

typedef struct Matriz {
    Indice filas;
    Indice columnas;
    Real datos[MAX_DIM][MAX_DIM];
} Matriz;

typedef struct Vector {
    Indice longitud;
    Real elementos[MAX_DIM];
} Vector;

typedef struct SistemaLineal {
    Matriz A;
    Vector b;
    Vector x;
    int resuelto;
} SistemaLineal;

/* Prototipos de funciones */
void matriz_inicializar(Matriz *m, Indice filas, Indice columnas);
void matriz_identidad(Matriz *m, Indice n);
void matriz_imprimir(const Matriz *m);
int matriz_copiar(Matriz *dest, const Matriz *orig);
int matriz_sumar(Matriz *res, const Matriz *a, const Matriz *b);
int matriz_restar(Matriz *res, const Matriz *a, const Matriz *b);
int matriz_multiplicar(Matriz *res, const Matriz *a, const Matriz *b);
int matriz_transponer(Matriz *res, const Matriz *orig);
Real matriz_traza(const Matriz *m);
Real matriz_determinante(const Matriz *m);
int matriz_inversa_gauss(Matriz *inv, const Matriz *orig);
int matriz_descomposicion_lu(const Matriz *A, Matriz *L, Matriz *U);

void vector_inicializar(Vector *v, Indice longitud);
void vector_imprimir(const Vector *v);
Real vector_producto_punto(const Vector *a, const Vector *b);
Real vector_norma(const Vector *v);
int matriz_por_vector(Vector *res, const Matriz *m, const Vector *v);

int resolver_sistema_gauss(SistemaLineal *sis);
int resolver_triangular_inferior(const Matriz *L, const Vector *b, Vector *x);
int resolver_triangular_superior(const Matriz *U, const Vector *b, Vector *x);

Real valor_absoluto(Real val) {
    if (val < 0.0) {
        return -val;
    } else {
        return val;
    }
}

void matriz_inicializar(Matriz *m, Indice filas, Indice columnas) {
    Indice i, j;
    m->filas = filas;
    m->columnas = columnas;
    for (i = 0; i < filas; i = i + 1) {
        for (j = 0; j < columnas; j = j + 1) {
            m->datos[i][j] = 0.0;
        }
    }
}

void matriz_identidad(Matriz *m, Indice n) {
    Indice i, j;
    matriz_inicializar(m, n, n);
    for (i = 0; i < n; i = i + 1) {
        for (j = 0; j < n; j = j + 1) {
            if (i == j) {
                m->datos[i][j] = 1.0;
            } else {
                m->datos[i][j] = 0.0;
            }
        }
    }
}

int matriz_copiar(Matriz *dest, const Matriz *orig) {
    Indice i, j;
    dest->filas = orig->filas;
    dest->columnas = orig->columnas;
    for (i = 0; i < orig->filas; i = i + 1) {
        for (j = 0; j < orig->columnas; j = j + 1) {
            dest->datos[i][j] = orig->datos[i][j];
        }
    }
    return 1;
}

int matriz_sumar(Matriz *res, const Matriz *a, const Matriz *b) {
    Indice i, j;
    if (a->filas != b->filas || a->columnas != b->columnas) {
        return 0;
    }
    res->filas = a->filas;
    res->columnas = a->columnas;
    for (i = 0; i < a->filas; i = i + 1) {
        for (j = 0; j < a->columnas; j = j + 1) {
            res->datos[i][j] = a->datos[i][j] + b->datos[i][j];
        }
    }
    return 1;
}

int matriz_restar(Matriz *res, const Matriz *a, const Matriz *b) {
    Indice i, j;
    if (a->filas != b->filas || a->columnas != b->columnas) {
        return 0;
    }
    res->filas = a->filas;
    res->columnas = a->columnas;
    for (i = 0; i < a->filas; i = i + 1) {
        for (j = 0; j < a->columnas; j = j + 1) {
            res->datos[i][j] = a->datos[i][j] - b->datos[i][j];
        }
    }
    return 1;
}

int matriz_multiplicar(Matriz *res, const Matriz *a, const Matriz *b) {
    Indice i, j, k;
    Real suma;
    if (a->columnas != b->filas) {
        return 0;
    }
    res->filas = a->filas;
    res->columnas = b->columnas;
    for (i = 0; i < a->filas; i = i + 1) {
        for (j = 0; j < b->columnas; j = j + 1) {
            suma = 0.0;
            for (k = 0; k < a->columnas; k = k + 1) {
                suma = suma + a->datos[i][k] * b->datos[k][j];
            }
            res->datos[i][j] = suma;
        }
    }
    return 1;
}

int matriz_transponer(Matriz *res, const Matriz *orig) {
    Indice i, j;
    res->filas = orig->columnas;
    res->columnas = orig->filas;
    for (i = 0; i < orig->filas; i = i + 1) {
        for (j = 0; j < orig->columnas; j = j + 1) {
            res->datos[j][i] = orig->datos[i][j];
        }
    }
    return 1;
}

Real matriz_traza(const Matriz *m) {
    Indice i, n;
    Real tr;
    tr = 0.0;
    if (m->filas < m->columnas) {
        n = m->filas;
    } else {
        n = m->columnas;
    }
    for (i = 0; i < n; i = i + 1) {
        tr = tr + m->datos[i][i];
    }
    return tr;
}

Real matriz_determinante(const Matriz *m) {
    Matriz temp;
    Indice n, i, j, k, max_row;
    Real det, max_val, factor, pivote;

    if (m->filas != m->columnas) {
        return 0.0;
    }
    n = m->filas;
    matriz_copiar(&temp, m);
    det = 1.0;

    for (i = 0; i < n; i = i + 1) {
        max_row = i;
        max_val = valor_absoluto(temp.datos[i][i]);
        for (k = i + 1; k < n; k = k + 1) {
            if (valor_absoluto(temp.datos[k][i]) > max_val) {
                max_val = valor_absoluto(temp.datos[k][i]);
                max_row = k;
            }
        }

        if (max_val < EPSILON) {
            return 0.0;
        }

        if (max_row != i) {
            for (j = 0; j < n; j = j + 1) {
                Real intercambio = temp.datos[i][j];
                temp.datos[i][j] = temp.datos[max_row][j];
                temp.datos[max_row][j] = intercambio;
            }
            det = -det;
        }

        pivote = temp.datos[i][i];
        det = det * pivote;

        for (k = i + 1; k < n; k = k + 1) {
            factor = temp.datos[k][i] / pivote;
            for (j = i; j < n; j = j + 1) {
                temp.datos[k][j] = temp.datos[k][j] - factor * temp.datos[i][j];
            }
        }
    }
    return det;
}

int matriz_inversa_gauss(Matriz *inv, const Matriz *orig) {
    Matriz a;
    Indice n, i, j, k, max_row;
    Real max_val, pivote, factor;

    if (orig->filas != orig->columnas) {
        return 0;
    }
    n = orig->filas;
    matriz_copiar(&a, orig);
    matriz_identidad(inv, n);

    for (i = 0; i < n; i = i + 1) {
        max_row = i;
        max_val = valor_absoluto(a.datos[i][i]);
        for (k = i + 1; k < n; k = k + 1) {
            if (valor_absoluto(a.datos[k][i]) > max_val) {
                max_val = valor_absoluto(a.datos[k][i]);
                max_row = k;
            }
        }

        if (max_val < EPSILON) {
            return 0;
        }

        if (max_row != i) {
            for (j = 0; j < n; j = j + 1) {
                Real t1 = a.datos[i][j];
                Real t2 = inv->datos[i][j];
                a.datos[i][j] = a.datos[max_row][j];
                a.datos[max_row][j] = t1;
                inv->datos[i][j] = inv->datos[max_row][j];
                inv->datos[max_row][j] = t2;
            }
        }

        pivote = a.datos[i][i];
        for (j = 0; j < n; j = j + 1) {
            a.datos[i][j] = a.datos[i][j] / pivote;
            inv->datos[i][j] = inv->datos[i][j] / pivote;
        }

        for (k = 0; k < n; k = k + 1) {
            if (k != i) {
                factor = a.datos[k][i];
                for (j = 0; j < n; j = j + 1) {
                    a.datos[k][j] = a.datos[k][j] - factor * a.datos[i][j];
                    inv->datos[k][j] = inv->datos[k][j] - factor * inv->datos[i][j];
                }
            }
        }
    }
    return 1;
}

int matriz_descomposicion_lu(const Matriz *A, Matriz *L, Matriz *U) {
    Indice n, i, j, k;
    Real suma;

    if (A->filas != A->columnas) {
        return 0;
    }
    n = A->filas;
    matriz_inicializar(L, n, n);
    matriz_inicializar(U, n, n);

    for (i = 0; i < n; i = i + 1) {
        L->datos[i][i] = 1.0;
        for (j = i; j < n; j = j + 1) {
            suma = 0.0;
            for (k = 0; k < i; k = k + 1) {
                suma = suma + L->datos[i][k] * U->datos[k][j];
            }
            U->datos[i][j] = A->datos[i][j] - suma;
        }
        for (j = i + 1; j < n; j = j + 1) {
            suma = 0.0;
            for (k = 0; k < i; k = k + 1) {
                suma = suma + L->datos[j][k] * U->datos[k][i];
            }
            if (valor_absoluto(U->datos[i][i]) < EPSILON) {
                return 0;
            }
            L->datos[j][i] = (A->datos[j][i] - suma) / U->datos[i][i];
        }
    }
    return 1;
}

void vector_inicializar(Vector *v, Indice longitud) {
    Indice i;
    v->longitud = longitud;
    for (i = 0; i < longitud; i = i + 1) {
        v->elementos[i] = 0.0;
    }
}

Real vector_producto_punto(const Vector *a, const Vector *b) {
    Indice i;
    Real res;
    res = 0.0;
    if (a->longitud != b->longitud) {
        return 0.0;
    }
    for (i = 0; i < a->longitud; i = i + 1) {
        res = res + a->elementos[i] * b->elementos[i];
    }
    return res;
}

Real vector_norma(const Vector *v) {
    Real suma;
    Indice i;
    suma = 0.0;
    for (i = 0; i < v->longitud; i = i + 1) {
        suma = suma + v->elementos[i] * v->elementos[i];
    }
    return suma;
}

int matriz_por_vector(Vector *res, const Matriz *m, const Vector *v) {
    Indice i, j;
    Real suma;
    if (m->columnas != v->longitud) {
        return 0;
    }
    res->longitud = m->filas;
    for (i = 0; i < m->filas; i = i + 1) {
        suma = 0.0;
        for (j = 0; j < m->columnas; j = j + 1) {
            suma = suma + m->datos[i][j] * v->elementos[j];
        }
        res->elementos[i] = suma;
    }
    return 1;
}

int resolver_triangular_inferior(const Matriz *L, const Vector *b, Vector *x) {
    Indice i, j, n;
    Real suma;
    n = L->filas;
    x->longitud = n;
    for (i = 0; i < n; i = i + 1) {
        suma = 0.0;
        for (j = 0; j < i; j = j + 1) {
            suma = suma + L->datos[i][j] * x->elementos[j];
        }
        if (valor_absoluto(L->datos[i][i]) < EPSILON) {
            return 0;
        }
        x->elementos[i] = (b->elementos[i] - suma) / L->datos[i][i];
    }
    return 1;
}

int resolver_triangular_superior(const Matriz *U, const Vector *b, Vector *x) {
    Indice j, n;
    int i;
    Real suma;
    n = U->filas;
    x->longitud = n;
    for (i = (int)n - 1; i >= 0; i = i - 1) {
        suma = 0.0;
        for (j = (Indice)(i + 1); j < n; j = j + 1) {
            suma = suma + U->datos[i][j] * x->elementos[j];
        }
        if (valor_absoluto(U->datos[i][i]) < EPSILON) {
            return 0;
        }
        x->elementos[i] = (b->elementos[i] - suma) / U->datos[i][i];
    }
    return 1;
}

int resolver_sistema_gauss(SistemaLineal *sis) {
    Matriz aug;
    Indice n, i, j, k, max_row;
    Real max_val, pivote, factor, suma;

    n = sis->A.filas;
    if (n != sis->A.columnas || n != sis->b.longitud) {
        sis->resuelto = 0;
        return 0;
    }

    aug.filas = n;
    aug.columnas = n + 1;
    for (i = 0; i < n; i = i + 1) {
        for (j = 0; j < n; j = j + 1) {
            aug.datos[i][j] = sis->A.datos[i][j];
        }
        aug.datos[i][n] = sis->b.elementos[i];
    }

    for (i = 0; i < n; i = i + 1) {
        max_row = i;
        max_val = valor_absoluto(aug.datos[i][i]);
        for (k = i + 1; k < n; k = k + 1) {
            if (valor_absoluto(aug.datos[k][i]) > max_val) {
                max_val = valor_absoluto(aug.datos[k][i]);
                max_row = k;
            }
        }
        if (max_val < EPSILON) {
            sis->resuelto = 0;
            return 0;
        }
        if (max_row != i) {
            for (j = 0; j <= n; j = j + 1) {
                Real temp = aug.datos[i][j];
                aug.datos[i][j] = aug.datos[max_row][j];
                aug.datos[max_row][j] = temp;
            }
        }
        pivote = aug.datos[i][i];
        for (k = i + 1; k < n; k = k + 1) {
            factor = aug.datos[k][i] / pivote;
            for (j = i; j <= n; j = j + 1) {
                aug.datos[k][j] = aug.datos[k][j] - factor * aug.datos[i][j];
            }
        }
    }

    sis->x.longitud = n;
    for (i = n - 1; (int)i >= 0; i = i - 1) {
        suma = 0.0;
        for (j = i + 1; j < n; j = j + 1) {
            suma = suma + aug.datos[i][j] * sis->x.elementos[j];
        }
        sis->x.elementos[i] = (aug.datos[i][n] - suma) / aug.datos[i][i];
        if (i == 0) break;
    }

    sis->resuelto = 1;
    return 1;
}

int prueba_algebra_lineal(void) {
    Matriz A, B, C, Inv;
    Vector v1, v2, v3;
    SistemaLineal sistema;
    Real det, traza;
    int ok;

    matriz_inicializar(&A, 3, 3);
    A.datos[0][0] = 2.0; A.datos[0][1] = 1.0; A.datos[0][2] = -1.0;
    A.datos[1][0] = -3.0; A.datos[1][1] = -1.0; A.datos[1][2] = 2.0;
    A.datos[2][0] = -2.0; A.datos[2][1] = 1.0; A.datos[2][2] = 2.0;

    det = matriz_determinante(&A);
    traza = matriz_traza(&A);

    if (valor_absoluto(det) > EPSILON) {
        ok = matriz_inversa_gauss(&Inv, &A);
        if (ok) {
            matriz_multiplicar(&C, &A, &Inv);
        }
    }

    vector_inicializar(&v1, 3);
    v1.elementos[0] = 8.0;
    v1.elementos[1] = -11.0;
    v1.elementos[2] = -3.0;

    sistema.A = A;
    sistema.b = v1;
    sistema.resuelto = 0;

    resolver_sistema_gauss(&sistema);

    matriz_identidad(&B, 3);
    matriz_sumar(&C, &A, &B);
    matriz_restar(&C, &A, &B);
    matriz_transponer(&C, &A);

    vector_inicializar(&v2, 3);
    v2.elementos[0] = 1.0;
    v2.elementos[1] = 2.0;
    v2.elementos[2] = 3.0;

    matriz_por_vector(&v3, &A, &v2);
    vector_producto_punto(&v1, &v2);
    vector_norma(&v3);

    return 0;
}

int main(void) {
    int resultado;
    resultado = prueba_algebra_lineal();
    if (resultado == 0) {
        return 0;
    } else {
        return 1;
    }
}
