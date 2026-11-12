/*
 * tests/valid_2_datastructs.c
 * Estructuras de datos avanzadas en ANSI C (C89):
 * Árbol AVL balanceado, Lista doblemente enlazada, Cola de prioridad (Heap)
 * y Tabla Hash con direccionamiento por encadenamiento.
 * Sin errores sintácticos.
 */

#define TABLA_TAM_MAX 128
#define HEAP_CAPACIDAD 256
#define CADENA_MAX 64

typedef int Clave;
typedef long Valor;

/* ------------------------------------------------------------- */
/*                      LISTA DOBLEMENTE ENLAZADA                */
/* ------------------------------------------------------------- */

typedef struct NodoLista {
    Clave clave;
    Valor valor;
    struct NodoLista *anterior;
    struct NodoLista *siguiente;
} NodoLista;

typedef struct ListaDoble {
    NodoLista *cabeza;
    NodoLista *cola;
    int longitud;
} ListaDoble;

void lista_inicializar(ListaDoble *lista);
int lista_insertar_inicio(ListaDoble *lista, Clave clave, Valor valor);
int lista_insertar_final(ListaDoble *lista, Clave clave, Valor valor);
int lista_eliminar_nodo(ListaDoble *lista, Clave clave);
NodoLista *lista_buscar(const ListaDoble *lista, Clave clave);
void lista_limpiar(ListaDoble *lista);

void lista_inicializar(ListaDoble *lista) {
    lista->cabeza = 0;
    lista->cola = 0;
    lista->longitud = 0;
}

int lista_insertar_final(ListaDoble *lista, Clave clave, Valor valor) {
    static NodoLista pool[512];
    static int pool_idx = 0;
    NodoLista *nuevo;

    if (pool_idx >= 512) {
        return 0;
    }
    nuevo = &pool[pool_idx];
    pool_idx = pool_idx + 1;

    nuevo->clave = clave;
    nuevo->valor = valor;
    nuevo->siguiente = 0;
    nuevo->anterior = lista->cola;

    if (lista->cola != 0) {
        lista->cola->siguiente = nuevo;
    } else {
        lista->cabeza = nuevo;
    }
    lista->cola = nuevo;
    lista->longitud = lista->longitud + 1;
    return 1;
}

int lista_insertar_inicio(ListaDoble *lista, Clave clave, Valor valor) {
    static NodoLista pool_ini[512];
    static int pool_ini_idx = 0;
    NodoLista *nuevo;

    if (pool_ini_idx >= 512) {
        return 0;
    }
    nuevo = &pool_ini[pool_ini_idx];
    pool_ini_idx = pool_ini_idx + 1;

    nuevo->clave = clave;
    nuevo->valor = valor;
    nuevo->anterior = 0;
    nuevo->siguiente = lista->cabeza;

    if (lista->cabeza != 0) {
        lista->cabeza->anterior = nuevo;
    } else {
        lista->cola = nuevo;
    }
    lista->cabeza = nuevo;
    lista->longitud = lista->longitud + 1;
    return 1;
}

NodoLista *lista_buscar(const ListaDoble *lista, Clave clave) {
    NodoLista *actual;
    actual = lista->cabeza;
    while (actual != 0) {
        if (actual->clave == clave) {
            return actual;
        }
        actual = actual->siguiente;
    }
    return 0;
}

int lista_eliminar_nodo(ListaDoble *lista, Clave clave) {
    NodoLista *nodo;
    nodo = lista_buscar(lista, clave);
    if (nodo == 0) {
        return 0;
    }
    if (nodo->anterior != 0) {
        nodo->anterior->siguiente = nodo->siguiente;
    } else {
        lista->cabeza = nodo->siguiente;
    }
    if (nodo->siguiente != 0) {
        nodo->siguiente->anterior = nodo->anterior;
    } else {
        lista->cola = nodo->anterior;
    }
    lista->longitud = lista->longitud - 1;
    return 1;
}

void lista_limpiar(ListaDoble *lista) {
    lista->cabeza = 0;
    lista->cola = 0;
    lista->longitud = 0;
}

/* ------------------------------------------------------------- */
/*                         ÁRBOL AVL                             */
/* ------------------------------------------------------------- */

typedef struct NodoAVL {
    Clave clave;
    Valor valor;
    int altura;
    struct NodoAVL *izquierdo;
    struct NodoAVL *derecho;
} NodoAVL;

typedef struct ArbolAVL {
    NodoAVL *raiz;
    int total_nodos;
} ArbolAVL;

int maximo(int a, int b) {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}

int avl_altura(const NodoAVL *n) {
    if (n == 0) {
        return 0;
    }
    return n->altura;
}

int avl_factor_balance(const NodoAVL *n) {
    if (n == 0) {
        return 0;
    }
    return avl_altura(n->izquierdo) - avl_altura(n->derecho);
}

NodoAVL *avl_crear_nodo(Clave clave, Valor valor) {
    static NodoAVL pool_avl[512];
    static int avl_idx = 0;
    NodoAVL *nuevo;

    if (avl_idx >= 512) {
        return 0;
    }
    nuevo = &pool_avl[avl_idx];
    avl_idx = avl_idx + 1;

    nuevo->clave = clave;
    nuevo->valor = valor;
    nuevo->altura = 1;
    nuevo->izquierdo = 0;
    nuevo->derecho = 0;
    return nuevo;
}

NodoAVL *avl_rotar_derecha(NodoAVL *y) {
    NodoAVL *x;
    NodoAVL *T2;

    x = y->izquierdo;
    T2 = x->derecho;

    x->derecho = y;
    y->izquierdo = T2;

    y->altura = maximo(avl_altura(y->izquierdo), avl_altura(y->derecho)) + 1;
    x->altura = maximo(avl_altura(x->izquierdo), avl_altura(x->derecho)) + 1;

    return x;
}

NodoAVL *avl_rotar_izquierda(NodoAVL *x) {
    NodoAVL *y;
    NodoAVL *T2;

    y = x->derecho;
    T2 = y->izquierdo;

    y->izquierdo = x;
    x->derecho = T2;

    x->altura = maximo(avl_altura(x->izquierdo), avl_altura(x->derecho)) + 1;
    y->altura = maximo(avl_altura(y->izquierdo), avl_altura(y->derecho)) + 1;

    return y;
}

NodoAVL *avl_insertar(NodoAVL *nodo, Clave clave, Valor valor) {
    int balance;

    if (nodo == 0) {
        return avl_crear_nodo(clave, valor);
    }

    if (clave < nodo->clave) {
        nodo->izquierdo = avl_insertar(nodo->izquierdo, clave, valor);
    } else if (clave > nodo->clave) {
        nodo->derecho = avl_insertar(nodo->derecho, clave, valor);
    } else {
        nodo->valor = valor;
        return nodo;
    }

    nodo->altura = 1 + maximo(avl_altura(nodo->izquierdo), avl_altura(nodo->derecho));
    balance = avl_factor_balance(nodo);

    /* Caso Izquierda-Izquierda */
    if (balance > 1 && clave < nodo->izquierdo->clave) {
        return avl_rotar_derecha(nodo);
    }

    /* Caso Derecha-Derecha */
    if (balance < -1 && clave > nodo->derecho->clave) {
        return avl_rotar_izquierda(nodo);
    }

    /* Caso Izquierda-Derecha */
    if (balance > 1 && clave > nodo->izquierdo->clave) {
        nodo->izquierdo = avl_rotar_izquierda(nodo->izquierdo);
        return avl_rotar_derecha(nodo);
    }

    /* Caso Derecha-Izquierda */
    if (balance < -1 && clave < nodo->derecho->clave) {
        nodo->derecho = avl_rotar_derecha(nodo->derecho);
        return avl_rotar_izquierda(nodo);
    }

    return nodo;
}

NodoAVL *avl_nodo_valor_minimo(NodoAVL *nodo) {
    NodoAVL *actual;
    actual = nodo;
    while (actual->izquierdo != 0) {
        actual = actual->izquierdo;
    }
    return actual;
}

NodoAVL *avl_eliminar(NodoAVL *raiz, Clave clave) {
    int balance;
    NodoAVL *temp;

    if (raiz == 0) {
        return raiz;
    }

    if (clave < raiz->clave) {
        raiz->izquierdo = avl_eliminar(raiz->izquierdo, clave);
    } else if (clave > raiz->clave) {
        raiz->derecho = avl_eliminar(raiz->derecho, clave);
    } else {
        if (raiz->izquierdo == 0 || raiz->derecho == 0) {
            if (raiz->izquierdo != 0) {
                temp = raiz->izquierdo;
            } else {
                temp = raiz->derecho;
            }

            if (temp == 0) {
                temp = raiz;
                raiz = 0;
            } else {
                *raiz = *temp;
            }
        } else {
            temp = avl_nodo_valor_minimo(raiz->derecho);
            raiz->clave = temp->clave;
            raiz->valor = temp->valor;
            raiz->derecho = avl_eliminar(raiz->derecho, temp->clave);
        }
    }

    if (raiz == 0) {
        return raiz;
    }

    raiz->altura = 1 + maximo(avl_altura(raiz->izquierdo), avl_altura(raiz->derecho));
    balance = avl_factor_balance(raiz);

    if (balance > 1 && avl_factor_balance(raiz->izquierdo) >= 0) {
        return avl_rotar_derecha(raiz);
    }

    if (balance > 1 && avl_factor_balance(raiz->izquierdo) < 0) {
        raiz->izquierdo = avl_rotar_izquierda(raiz->izquierdo);
        return avl_rotar_derecha(raiz);
    }

    if (balance < -1 && avl_factor_balance(raiz->derecho) <= 0) {
        return avl_rotar_izquierda(raiz);
    }

    if (balance < -1 && avl_factor_balance(raiz->derecho) > 0) {
        raiz->derecho = avl_rotar_derecha(raiz->derecho);
        return avl_rotar_izquierda(raiz);
    }

    return raiz;
}

NodoAVL *avl_buscar(NodoAVL *raiz, Clave clave) {
    if (raiz == 0 || raiz->clave == clave) {
        return raiz;
    }
    if (clave < raiz->clave) {
        return avl_buscar(raiz->izquierdo, clave);
    }
    return avl_buscar(raiz->derecho, clave);
}

/* ------------------------------------------------------------- */
/*                    COLA DE PRIORIDAD (HEAP)                   */
/* ------------------------------------------------------------- */

typedef struct ElementoHeap {
    int prioridad;
    Valor dato;
} ElementoHeap;

typedef struct ColaPrioridad {
    ElementoHeap elementos[HEAP_CAPACIDAD];
    int tamano;
} ColaPrioridad;

void heap_inicializar(ColaPrioridad *h) {
    h->tamano = 0;
}

int heap_padre(int i) {
    return (i - 1) / 2;
}

int heap_hijo_izq(int i) {
    return 2 * i + 1;
}

int heap_hijo_der(int i) {
    return 2 * i + 2;
}

void heap_intercambiar(ElementoHeap *a, ElementoHeap *b) {
    ElementoHeap temp;
    temp = *a;
    *a = *b;
    *b = temp;
}

int heap_insertar(ColaPrioridad *h, int prioridad, Valor dato) {
    int i;
    if (h->tamano >= HEAP_CAPACIDAD) {
        return 0;
    }
    i = h->tamano;
    h->tamano = h->tamano + 1;
    h->elementos[i].prioridad = prioridad;
    h->elementos[i].dato = dato;

    while (i != 0 && h->elementos[heap_padre(i)].prioridad < h->elementos[i].prioridad) {
        heap_intercambiar(&h->elementos[i], &h->elementos[heap_padre(i)]);
        i = heap_padre(i);
    }
    return 1;
}

void heapify(ColaPrioridad *h, int i) {
    int mayor;
    int izq;
    int der;

    mayor = i;
    izq = heap_hijo_izq(i);
    der = heap_hijo_der(i);

    if (izq < h->tamano && h->elementos[izq].prioridad > h->elementos[mayor].prioridad) {
        mayor = izq;
    }
    if (der < h->tamano && h->elementos[der].prioridad > h->elementos[mayor].prioridad) {
        mayor = der;
    }
    if (mayor != i) {
        heap_intercambiar(&h->elementos[i], &h->elementos[mayor]);
        heapify(h, mayor);
    }
}

int heap_extraer_max(ColaPrioridad *h, ElementoHeap *resultado) {
    if (h->tamano <= 0) {
        return 0;
    }
    if (h->tamano == 1) {
        h->tamano = 0;
        *resultado = h->elementos[0];
        return 1;
    }
    *resultado = h->elementos[0];
    h->elementos[0] = h->elementos[h->tamano - 1];
    h->tamano = h->tamano - 1;
    heapify(h, 0);
    return 1;
}

/* ------------------------------------------------------------- */
/*                         TABLA HASH                            */
/* ------------------------------------------------------------- */

typedef struct EntradaHash {
    Clave clave;
    Valor valor;
    int ocupado;
    struct EntradaHash *siguiente;
} EntradaHash;

typedef struct TablaHash {
    EntradaHash cubetas[TABLA_TAM_MAX];
    int cantidad;
} TablaHash;

int hash_funcion(Clave clave) {
    int h;
    h = clave % TABLA_TAM_MAX;
    if (h < 0) {
        h = h + TABLA_TAM_MAX;
    }
    return h;
}

void hash_inicializar(TablaHash *t) {
    int i;
    t->cantidad = 0;
    for (i = 0; i < TABLA_TAM_MAX; i = i + 1) {
        t->cubetas[i].ocupado = 0;
        t->cubetas[i].siguiente = 0;
        t->cubetas[i].clave = 0;
        t->cubetas[i].valor = 0;
    }
}

int hash_insertar(TablaHash *t, Clave clave, Valor valor) {
    static EntradaHash pool_hash[512];
    static int hash_pool_idx = 0;
    int idx;
    EntradaHash *actual;

    idx = hash_funcion(clave);
    if (t->cubetas[idx].ocupado == 0) {
        t->cubetas[idx].clave = clave;
        t->cubetas[idx].valor = valor;
        t->cubetas[idx].ocupado = 1;
        t->cubetas[idx].siguiente = 0;
        t->cantidad = t->cantidad + 1;
        return 1;
    }

    actual = &t->cubetas[idx];
    while (actual != 0) {
        if (actual->clave == clave) {
            actual->valor = valor;
            return 1;
        }
        if (actual->siguiente == 0) {
            break;
        }
        actual = actual->siguiente;
    }

    if (hash_pool_idx >= 512) {
        return 0;
    }
    actual->siguiente = &pool_hash[hash_pool_idx];
    hash_pool_idx = hash_pool_idx + 1;

    actual->siguiente->clave = clave;
    actual->siguiente->valor = valor;
    actual->siguiente->ocupado = 1;
    actual->siguiente->siguiente = 0;
    t->cantidad = t->cantidad + 1;
    return 1;
}

int hash_buscar(const TablaHash *t, Clave clave, Valor *valor_salida) {
    int idx;
    const EntradaHash *actual;

    idx = hash_funcion(clave);
    if (t->cubetas[idx].ocupado == 0) {
        return 0;
    }

    actual = &t->cubetas[idx];
    while (actual != 0) {
        if (actual->clave == clave) {
            *valor_salida = actual->valor;
            return 1;
        }
        actual = actual->siguiente;
    }
    return 0;
}

/* ------------------------------------------------------------- */
/*                         FUNCIÓN PRINCIPAL                     */
/* ------------------------------------------------------------- */

int main(void) {
    ListaDoble lista;
    ArbolAVL arbol;
    ColaPrioridad heap;
    TablaHash tabla;
    ElementoHeap elem;
    Valor val_encontrado;
    int i;

    lista_inicializar(&lista);
    for (i = 1; i <= 20; i = i + 1) {
        lista_insertar_final(&lista, i, (Valor)(i * 10));
    }
    lista_eliminar_nodo(&lista, 5);
    lista_eliminar_nodo(&lista, 10);

    arbol.raiz = 0;
    arbol.total_nodos = 0;
    for (i = 1; i <= 15; i = i + 1) {
        arbol.raiz = avl_insertar(arbol.raiz, i * 3, (Valor)(i * 30));
    }
    arbol.raiz = avl_eliminar(arbol.raiz, 9);
    arbol.raiz = avl_eliminar(arbol.raiz, 15);

    heap_inicializar(&heap);
    heap_insertar(&heap, 50, 500);
    heap_insertar(&heap, 20, 200);
    heap_insertar(&heap, 80, 800);
    heap_insertar(&heap, 10, 100);
    heap_insertar(&heap, 90, 900);
    while (heap_extraer_max(&heap, &elem)) {
        /* Extraer todos los elementos en orden de prioridad */
    }

    hash_inicializar(&tabla);
    for (i = 100; i < 120; i = i + 1) {
        hash_insertar(&tabla, i, (Valor)(i * 100));
    }
    hash_buscar(&tabla, 105, &val_encontrado);

    return 0;
}
