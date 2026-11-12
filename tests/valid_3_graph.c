/*
 * tests/valid_3_graph.c
 * Algoritmos avanzados de teoría de grafos en ANSI C (C89):
 * BFS, DFS, Dijkstra, Prim (MST), Kruskal con Disjoint-Set (Union-Find)
 * y Ordenamiento Topológico.
 * Sin errores sintácticos.
 */

#define MAX_VERTICES 32
#define MAX_ARISTAS 256
#define INF 999999

typedef int Vertice;
typedef int Peso;

typedef struct Arista {
    Vertice origen;
    Vertice destino;
    Peso peso;
} Arista;

typedef struct GrafoMatriz {
    int num_vertices;
    int num_aristas;
    Peso matriz[MAX_VERTICES][MAX_VERTICES];
} GrafoMatriz;

typedef struct NodoAdy {
    Vertice destino;
    Peso peso;
    struct NodoAdy *siguiente;
} NodoAdy;

typedef struct GrafoLista {
    int num_vertices;
    int num_aristas;
    NodoAdy *listas[MAX_VERTICES];
} GrafoLista;

typedef struct ConjuntoDisjunto {
    int padre[MAX_VERTICES];
    int rango[MAX_VERTICES];
    int cantidad;
} ConjuntoDisjunto;

typedef struct Cola {
    int elementos[MAX_VERTICES * 2];
    int frente;
    int fin;
} Cola;

typedef struct Pila {
    int elementos[MAX_VERTICES * 2];
    int tope;
} Pila;

/* Declaraciones de funciones */
void grafo_matriz_init(GrafoMatriz *g, int n);
void grafo_matriz_agregar_arista(GrafoMatriz *g, Vertice u, Vertice v, Peso w, int dirigido);
void grafo_lista_init(GrafoLista *g, int n);
void grafo_lista_agregar_arista(GrafoLista *g, Vertice u, Vertice v, Peso w, int dirigido);

void cola_init(Cola *c);
int cola_vacia(const Cola *c);
void cola_encolar(Cola *c, int val);
int cola_desencolar(Cola *c);

void pila_init(Pila *p);
int pila_vacia(const Pila *p);
void pila_apilar(Pila *p, int val);
int pila_desapilar(Pila *p);

void dsu_init(ConjuntoDisjunto *dsu, int n);
int dsu_encontrar(ConjuntoDisjunto *dsu, int i);
void dsu_unir(ConjuntoDisjunto *dsu, int x, int y);

void bfs(const GrafoMatriz *g, Vertice inicio, int visitado[]);
void dfs_recursivo(const GrafoMatriz *g, Vertice u, int visitado[]);
void dijkstra(const GrafoMatriz *g, Vertice origen, Peso distancias[], int previo[]);
Peso prim_mst(const GrafoMatriz *g, int padre_mst[]);
Peso kruskal_mst(int num_vertices, Arista aristas[], int total_aristas, Arista resultado_mst[]);
int orden_topologico(const GrafoMatriz *g, int orden[]);

/* Implementación de Cola */
void cola_init(Cola *c) {
    c->frente = 0;
    c->fin = 0;
}

int cola_vacia(const Cola *c) {
    if (c->frente == c->fin) {
        return 1;
    } else {
        return 0;
    }
}

void cola_encolar(Cola *c, int val) {
    if (c->fin < MAX_VERTICES * 2) {
        c->elementos[c->fin] = val;
        c->fin = c->fin + 1;
    }
}

int cola_desencolar(Cola *c) {
    int val;
    if (!cola_vacia(c)) {
        val = c->elementos[c->frente];
        c->frente = c->frente + 1;
        return val;
    }
    return -1;
}

/* Implementación de Pila */
void pila_init(Pila *p) {
    p->tope = -1;
}

int pila_vacia(const Pila *p) {
    if (p->tope < 0) {
        return 1;
    } else {
        return 0;
    }
}

void pila_apilar(Pila *p, int val) {
    if (p->tope < MAX_VERTICES * 2 - 1) {
        p->tope = p->tope + 1;
        p->elementos[p->tope] = val;
    }
}

int pila_desapilar(Pila *p) {
    int val;
    if (!pila_vacia(p)) {
        val = p->elementos[p->tope];
        p->tope = p->tope - 1;
        return val;
    }
    return -1;
}

/* Implementación de Grafo con Matriz de Adyacencia */
void grafo_matriz_init(GrafoMatriz *g, int n) {
    int i, j;
    g->num_vertices = n;
    g->num_aristas = 0;
    for (i = 0; i < n; i = i + 1) {
        for (j = 0; j < n; j = j + 1) {
            if (i == j) {
                g->matriz[i][j] = 0;
            } else {
                g->matriz[i][j] = INF;
            }
        }
    }
}

void grafo_matriz_agregar_arista(GrafoMatriz *g, Vertice u, Vertice v, Peso w, int dirigido) {
    if (u >= 0 && u < g->num_vertices && v >= 0 && v < g->num_vertices) {
        g->matriz[u][v] = w;
        if (!dirigido) {
            g->matriz[v][u] = w;
        }
        g->num_aristas = g->num_aristas + 1;
    }
}

/* Implementación de Grafo con Lista de Adyacencia */
void grafo_lista_init(GrafoLista *g, int n) {
    int i;
    g->num_vertices = n;
    g->num_aristas = 0;
    for (i = 0; i < n; i = i + 1) {
        g->listas[i] = 0;
    }
}

void grafo_lista_agregar_arista(GrafoLista *g, Vertice u, Vertice v, Peso w, int dirigido) {
    static NodoAdy pool_nodos[512];
    static int nodo_idx = 0;
    NodoAdy *nuevo;

    if (nodo_idx >= 512) {
        return;
    }
    nuevo = &pool_nodos[nodo_idx];
    nodo_idx = nodo_idx + 1;

    nuevo->destino = v;
    nuevo->peso = w;
    nuevo->siguiente = g->listas[u];
    g->listas[u] = nuevo;

    if (!dirigido) {
        if (nodo_idx >= 512) {
            return;
        }
        nuevo = &pool_nodos[nodo_idx];
        nodo_idx = nodo_idx + 1;

        nuevo->destino = u;
        nuevo->peso = w;
        nuevo->siguiente = g->listas[v];
        g->listas[v] = nuevo;
    }
    g->num_aristas = g->num_aristas + 1;
}

/* Disjoint-Set Union (Union-Find) */
void dsu_init(ConjuntoDisjunto *dsu, int n) {
    int i;
    dsu->cantidad = n;
    for (i = 0; i < n; i = i + 1) {
        dsu->padre[i] = i;
        dsu->rango[i] = 0;
    }
}

int dsu_encontrar(ConjuntoDisjunto *dsu, int i) {
    if (dsu->padre[i] != i) {
        dsu->padre[i] = dsu_encontrar(dsu, dsu->padre[i]);
    }
    return dsu->padre[i];
}

void dsu_unir(ConjuntoDisjunto *dsu, int x, int y) {
    int raiz_x;
    int raiz_y;

    raiz_x = dsu_encontrar(dsu, x);
    raiz_y = dsu_encontrar(dsu, y);

    if (raiz_x != raiz_y) {
        if (dsu->rango[raiz_x] < dsu->rango[raiz_y]) {
            dsu->padre[raiz_x] = raiz_y;
        } else if (dsu->rango[raiz_x] > dsu->rango[raiz_y]) {
            dsu->padre[raiz_y] = raiz_x;
        } else {
            dsu->padre[raiz_y] = raiz_x;
            dsu->rango[raiz_x] = dsu->rango[raiz_x] + 1;
        }
    }
}

/* Recorrido BFS (Breadth-First Search) */
void bfs(const GrafoMatriz *g, Vertice inicio, int visitado[]) {
    Cola c;
    Vertice u, v;

    cola_init(&c);
    visitado[inicio] = 1;
    cola_encolar(&c, inicio);

    while (!cola_vacia(&c)) {
        u = cola_desencolar(&c);
        for (v = 0; v < g->num_vertices; v = v + 1) {
            if (g->matriz[u][v] != INF && g->matriz[u][v] != 0 && !visitado[v]) {
                visitado[v] = 1;
                cola_encolar(&c, v);
            }
        }
    }
}

/* Recorrido DFS (Depth-First Search) */
void dfs_recursivo(const GrafoMatriz *g, Vertice u, int visitado[]) {
    Vertice v;
    visitado[u] = 1;

    for (v = 0; v < g->num_vertices; v = v + 1) {
        if (g->matriz[u][v] != INF && g->matriz[u][v] != 0 && !visitado[v]) {
            dfs_recursivo(g, v, visitado);
        }
    }
}

/* Algoritmo de Dijkstra para Caminos Mínimos */
void dijkstra(const GrafoMatriz *g, Vertice origen, Peso distancias[], int previo[]) {
    int spt_set[MAX_VERTICES];
    int count, i, v, u, min_val, min_idx;

    for (i = 0; i < g->num_vertices; i = i + 1) {
        distancias[i] = INF;
        spt_set[i] = 0;
        previo[i] = -1;
    }

    distancias[origen] = 0;

    for (count = 0; count < g->num_vertices - 1; count = count + 1) {
        min_val = INF;
        min_idx = -1;

        for (v = 0; v < g->num_vertices; v = v + 1) {
            if (!spt_set[v] && distancias[v] <= min_val) {
                min_val = distancias[v];
                min_idx = v;
            }
        }

        if (min_idx == -1) {
            break;
        }

        u = min_idx;
        spt_set[u] = 1;

        for (v = 0; v < g->num_vertices; v = v + 1) {
            if (!spt_set[v] && g->matriz[u][v] != INF && distancias[u] != INF) {
                if (distancias[u] + g->matriz[u][v] < distancias[v]) {
                    distancias[v] = distancias[u] + g->matriz[u][v];
                    previo[v] = u;
                }
            }
        }
    }
}

/* Algoritmo de Prim para Árbol de Expansión Mínima (MST) */
Peso prim_mst(const GrafoMatriz *g, int padre_mst[]) {
    Peso clave[MAX_VERTICES];
    int mst_set[MAX_VERTICES];
    int i, count, v, u, min_val, min_idx;
    Peso peso_total;

    for (i = 0; i < g->num_vertices; i = i + 1) {
        clave[i] = INF;
        mst_set[i] = 0;
        padre_mst[i] = -1;
    }

    clave[0] = 0;
    padre_mst[0] = -1;

    for (count = 0; count < g->num_vertices - 1; count = count + 1) {
        min_val = INF;
        min_idx = -1;

        for (v = 0; v < g->num_vertices; v = v + 1) {
            if (!mst_set[v] && clave[v] < min_val) {
                min_val = clave[v];
                min_idx = v;
            }
        }

        if (min_idx == -1) {
            break;
        }

        u = min_idx;
        mst_set[u] = 1;

        for (v = 0; v < g->num_vertices; v = v + 1) {
            if (g->matriz[u][v] != INF && !mst_set[v] && g->matriz[u][v] < clave[v]) {
                padre_mst[v] = u;
                clave[v] = g->matriz[u][v];
            }
        }
    }

    peso_total = 0;
    for (i = 1; i < g->num_vertices; i = i + 1) {
        if (padre_mst[i] != -1) {
            peso_total = peso_total + g->matriz[i][padre_mst[i]];
        }
    }
    return peso_total;
}

/* Algoritmo de Kruskal con ordenamiento por inserción */
Peso kruskal_mst(int num_vertices, Arista aristas[], int total_aristas, Arista resultado_mst[]) {
    ConjuntoDisjunto dsu;
    int i, j, aristas_mst, u, v, raiz_u, raiz_v;
    Arista temp;
    Peso peso_total;

    /* Ordenar aristas por peso de menor a mayor */
    for (i = 1; i < total_aristas; i = i + 1) {
        temp = aristas[i];
        j = i - 1;
        while (j >= 0 && aristas[j].peso > temp.peso) {
            aristas[j + 1] = aristas[j];
            j = j - 1;
        }
        aristas[j + 1] = temp;
    }

    dsu_init(&dsu, num_vertices);
    aristas_mst = 0;
    peso_total = 0;

    for (i = 0; i < total_aristas && aristas_mst < num_vertices - 1; i = i + 1) {
        u = aristas[i].origen;
        v = aristas[i].destino;
        raiz_u = dsu_encontrar(&dsu, u);
        raiz_v = dsu_encontrar(&dsu, v);

        if (raiz_u != raiz_v) {
            resultado_mst[aristas_mst] = aristas[i];
            aristas_mst = aristas_mst + 1;
            peso_total = peso_total + aristas[i].peso;
            dsu_unir(&dsu, raiz_u, raiz_v);
        }
    }
    return peso_total;
}

/* Ordenamiento Topológico con DFS */
void dfs_topologico(const GrafoMatriz *g, Vertice u, int visitado[], Pila *p) {
    Vertice v;
    visitado[u] = 1;

    for (v = 0; v < g->num_vertices; v = v + 1) {
        if (g->matriz[u][v] != INF && g->matriz[u][v] != 0 && !visitado[v]) {
            dfs_topologico(g, v, visitado, p);
        }
    }
    pila_apilar(p, u);
}

int orden_topologico(const GrafoMatriz *g, int orden[]) {
    Pila p;
    int visitado[MAX_VERTICES];
    int i, idx;

    pila_init(&p);
    for (i = 0; i < g->num_vertices; i = i + 1) {
        visitado[i] = 0;
    }

    for (i = 0; i < g->num_vertices; i = i + 1) {
        if (!visitado[i]) {
            dfs_topologico(g, i, visitado, &p);
        }
    }

    idx = 0;
    while (!pila_vacia(&p)) {
        orden[idx] = pila_desapilar(&p);
        idx = idx + 1;
    }
    return idx;
}

/* Programa principal de pruebas */
int main(void) {
    GrafoMatriz gm;
    GrafoLista gl;
    int visitados_bfs[MAX_VERTICES];
    int visitados_dfs[MAX_VERTICES];
    Peso dist[MAX_VERTICES];
    int prev[MAX_VERTICES];
    int padre_mst[MAX_VERTICES];
    Arista aristas[10];
    Arista resultado_mst[MAX_VERTICES];
    int orden[MAX_VERTICES];
    int i;
    Peso peso_mst_prim;
    Peso peso_mst_kruskal;

    grafo_matriz_init(&gm, 6);
    grafo_matriz_agregar_arista(&gm, 0, 1, 7, 0);
    grafo_matriz_agregar_arista(&gm, 0, 2, 9, 0);
    grafo_matriz_agregar_arista(&gm, 0, 5, 14, 0);
    grafo_matriz_agregar_arista(&gm, 1, 2, 10, 0);
    grafo_matriz_agregar_arista(&gm, 1, 3, 15, 0);
    grafo_matriz_agregar_arista(&gm, 2, 3, 11, 0);
    grafo_matriz_agregar_arista(&gm, 2, 5, 2, 0);
    grafo_matriz_agregar_arista(&gm, 3, 4, 6, 0);
    grafo_matriz_agregar_arista(&gm, 4, 5, 9, 0);

    for (i = 0; i < 6; i = i + 1) {
        visitados_bfs[i] = 0;
        visitados_dfs[i] = 0;
    }

    bfs(&gm, 0, visitados_bfs);
    dfs_recursivo(&gm, 0, visitados_dfs);
    dijkstra(&gm, 0, dist, prev);
    peso_mst_prim = prim_mst(&gm, padre_mst);

    /* Preparar aristas para Kruskal */
    aristas[0].origen = 0; aristas[0].destino = 1; aristas[0].peso = 7;
    aristas[1].origen = 0; aristas[1].destino = 2; aristas[1].peso = 9;
    aristas[2].origen = 0; aristas[2].destino = 5; aristas[2].peso = 14;
    aristas[3].origen = 1; aristas[3].destino = 2; aristas[3].peso = 10;
    aristas[4].origen = 1; aristas[4].destino = 3; aristas[4].peso = 15;
    aristas[5].origen = 2; aristas[5].destino = 3; aristas[5].peso = 11;
    aristas[6].origen = 2; aristas[6].destino = 5; aristas[6].peso = 2;
    aristas[7].origen = 3; aristas[7].destino = 4; aristas[7].peso = 6;
    aristas[8].origen = 4; aristas[8].destino = 5; aristas[8].peso = 9;

    peso_mst_kruskal = kruskal_mst(6, aristas, 9, resultado_mst);

    /* Grafo dirigido para orden topológico */
    grafo_matriz_init(&gm, 6);
    grafo_matriz_agregar_arista(&gm, 5, 2, 1, 1);
    grafo_matriz_agregar_arista(&gm, 5, 0, 1, 1);
    grafo_matriz_agregar_arista(&gm, 4, 0, 1, 1);
    grafo_matriz_agregar_arista(&gm, 4, 1, 1, 1);
    grafo_matriz_agregar_arista(&gm, 2, 3, 1, 1);
    grafo_matriz_agregar_arista(&gm, 3, 1, 1, 1);

    orden_topologico(&gm, orden);

    grafo_lista_init(&gl, 4);
    grafo_lista_agregar_arista(&gl, 0, 1, 5, 1);
    grafo_lista_agregar_arista(&gl, 1, 2, 3, 1);
    grafo_lista_agregar_arista(&gl, 2, 3, 7, 1);

    return 0;
}
