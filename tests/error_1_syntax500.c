/*
 * tests/error_1_syntax500.c
 * Simulación física y astronómica de N-cuerpos en ANSI C (C89).
 * Contiene más de 500 líneas reales con errores sintácticos distribuidos.
 */

#define MAX_CUERPOS 64
#define PASO_DT 0.001
#define G_CTE 6.674e-11

typedef double Real;
typedef int Bool;

typedef struct Vector3 {
    Real x;
    Real y;
    Real z;
} Vector3;

typedef struct Cuerpo {
    Vector3 posicion;
    Vector3 velocidad;
    Vector3 fuerza;
    Real masa;
    Real radio;
    int id;
} Cuerpo;

typedef struct SistemaEstelar {
    Cuerpo cuerpos[MAX_CUERPOS];
    int total_cuerpos;
    Real tiempo;
    Real energia_inicial;
} SistemaEstelar;

/* Operaciones vectoriales */
Vector3 v_crear(Real x, Real y, Real z) {
    Vector3 r;
    r.x = x;
    r.y = y;
    r.z = z;
    return r;
}

Vector3 v_sumar(Vector3 a, Vector3 b) {
    Vector3 r;
    r.x = a.x + b.x;
    r.y = a.y + b.y;
    r.z = a.z + b.z;
    return r;
}

Vector3 v_restar(Vector3 a, Vector3 b) {
    Vector3 r;
    r.x = a.x - b.x;
    r.y = a.y - b.y;
    r.z = a.z - b.z;
    return r;
}

Vector3 v_mult(Vector3 a, Real c) {
    Vector3 r;
    r.x = a.x * c;
    r.y = a.y * c;
    r.z = a.z * c;
    return r;
}

Real v_norma_sq(Vector3 a) {
    return a.x * a.x + a.y * a.y + a.z * a.z;
}

Real v_distancia(Vector3 a, Vector3 b) {
    Vector3 d;
    d = v_restar(a, b);
    return v_norma_sq(d);
}

/* Inicialización del sistema estelar */
void sistema_iniciar(SistemaEstelar *s) {
    int i;
    s->total_cuerpos = 0;
    s->tiempo = 0.0;
    s->energia_inicial = 0.0;
    for (i = 0; i < MAX_CUERPOS; i = i + 1) {
        s->cuerpos[i].masa = 0.0;
        s->cuerpos[i].id = -1;
    }
}

int sistema_agregar_cuerpo(SistemaEstelar *s, Real masa, Real radio, Vector3 pos, Vector3 vel) {
    int idx;
    if (s->total_cuerpos >= MAX_CUERPOS) {
        return -1;
    }
    idx = s->total_cuerpos;
    s->cuerpos[idx].id = idx;
    s->cuerpos[idx].masa = masa;
    s->cuerpos[idx].radio = radio;
    s->cuerpos[idx].posicion = pos;
    s->cuerpos[idx].velocidad = vel;
    s->cuerpos[idx].fuerza = v_crear(0.0, 0.0, 0.0);
    s->total_cuerpos = s->total_cuerpos + 1;
    return idx;
}

/* Cálculo de fuerzas gravitacionales */
void calcular_fuerzas_gravitatorias(SistemaEstelar *s) {
    int i, j;
    Vector3 f_cero;
    f_cero = v_crear(0.0, 0.0, 0.0);

    for (i = 0; i < s->total_cuerpos; i = i + 1) {
        s->cuerpos[i].fuerza = f_cero;
    }

    for (i = 0; i < s->total_cuerpos; i = i + 1) {
        for (j = i + 1; j < s->total_cuerpos; j = j + 1) {
            Vector3 r;
            Real d2, d, f_mag;
            Vector3 f_dir;

            r = v_restar(s->cuerpos[j].posicion, s->cuerpos[i].posicion);
            d2 = v_norma_sq(r);
            if (d2 < 0.0001) {
                d2 = 0.0001;
            }

            /* ERROR SINTÁCTICO 1: Operadores aritméticos seguidos sin operando */
            f_mag = G_CTE * s->cuerpos[i].masa * / s->cuerpos[j].masa / d2;
            f_dir = v_mult(r, f_mag);

            s->cuerpos[i].fuerza = v_sumar(s->cuerpos[i].fuerza, f_dir);
            s->cuerpos[j].fuerza = v_restar(s->cuerpos[j].fuerza, f_dir);
        }
    }
}

/* Integración Verlet de velocidad */
void integracion_verlet(SistemaEstelar *s, Real dt) {
    int i;
    Vector3 a_ant, a_nueva;

    for (i = 0; i < s->total_cuerpos; i = i + 1) {
        if (s->cuerpos[i].masa <= 0.0) continue;
        a_ant = v_mult(s->cuerpos[i].fuerza, 1.0 / s->cuerpos[i].masa);

        /* ERROR SINTÁCTICO 2: Falta de punto y coma al final de sentencia */
        s->cuerpos[i].posicion = v_sumar(s->cuerpos[i].posicion, v_mult(s->cuerpos[i].velocidad, dt))
        s->cuerpos[i].posicion = v_sumar(s->cuerpos[i].posicion, v_mult(a_ant, 0.5 * dt * dt));
    }

    calcular_fuerzas_gravitatorias(s);

    for (i = 0; i < s->total_cuerpos; i = i + 1) {
        if (s->cuerpos[i].masa <= 0.0) continue;
        a_nueva = v_mult(s->cuerpos[i].fuerza, 1.0 / s->cuerpos[i].masa);
        s->cuerpos[i].velocidad = v_sumar(s->cuerpos[i].velocidad, v_mult(v_sumar(a_ant, a_nueva), 0.5 * dt));
    }
    s->tiempo = s->tiempo + dt;
}

/* Cálculo de energía total del sistema */
Real calcular_energia_total(const SistemaEstelar *s) {
    Real e_cinetica;
    Real e_potencial;
    int i, j;

    e_cinetica = 0.0;
    e_potencial = 0.0;

    for (i = 0; i < s->total_cuerpos; i = i + 1) {
        Real v2;
        v2 = v_norma_sq(s->cuerpos[i].velocidad);
        e_cinetica = e_cinetica + 0.5 * s->cuerpos[i].masa * v2;
    }

    for (i = 0; i < s->total_cuerpos; i = i + 1) {
        for (j = i + 1; j < s->total_cuerpos; j = j + 1) {
            Real dist;
            Vector3 r;
            r = v_restar(s->cuerpos[j].posicion, s->cuerpos[i].posicion);
            dist = v_norma_sq(r);
            if (dist > 0.0001) {
                e_potencial = e_potencial - (G_CTE * s->cuerpos[i].masa * s->cuerpos[j].masa) / dist;
            }
        }
    }

    /* ERROR SINTÁCTICO 3: Paréntesis desbalanceados en return */
    return (e_cinetica + e_potencial;
}

/* Detección y manejo de colisiones inelásticas */
int resolver_colisiones(SistemaEstelar *s) {
    int i, j, colisiones;
    colisiones = 0;

    for (i = 0; i < s->total_cuerpos; i = i + 1) {
        /* ERROR SINTÁCTICO 4: Sentencia if con condición incompleta */
        if (s->cuerpos[i].masa <= ) {
            continue;
        }
        for (j = i + 1; j < s->total_cuerpos; j = j + 1) {
            Real d_limite, d_real;
            Vector3 r;
            if (s->cuerpos[j].masa <= 0.0) continue;

            r = v_restar(s->cuerpos[j].posicion, s->cuerpos[i].posicion);
            d_real = v_norma_sq(r);
            d_limite = s->cuerpos[i].radio + s->cuerpos[j].radio;

            if (d_real < d_limite * d_limite) {
                Real m_total;
                Vector3 p_total;
                m_total = s->cuerpos[i].masa + s->cuerpos[j].masa;
                p_total = v_sumar(v_mult(s->cuerpos[i].velocidad, s->cuerpos[i].masa),
                                  v_mult(s->cuerpos[j].velocidad, s->cuerpos[j].masa));

                s->cuerpos[i].velocidad = v_mult(p_total, 1.0 / m_total);
                s->cuerpos[i].masa = m_total;
                s->cuerpos[j].masa = 0.0;
                colisiones = colisiones + 1;
            }
        }
    }
    return colisiones;
}

/* Rutina de propagación orbital de Runge-Kutta 4 */
typedef struct DerivadaOrbit {
    Vector3 dr;
    Vector3 dv;
} DerivadaOrbit;

DerivadaOrbit rk_evaluar(Vector3 r_ini, Vector3 v_ini, Real dt, DerivadaOrbit d, Vector3 g_acc) {
    DerivadaOrbit sal;
    Vector3 r_temp;
    Vector3 v_temp;

    r_temp = v_sumar(r_ini, v_mult(d.dr, dt));
    v_temp = v_sumar(v_ini, v_mult(d.dv, dt));

    sal.dr = v_temp;
    sal.dv = g_acc;
    return sal;
}

void rk_paso_cuerpo(Cuerpo *c, Real dt, Vector3 g_acc) {
    DerivadaOrbit d0, a, b, c_step, d;
    Vector3 dr, dv;

    d0.dr = v_crear(0.0, 0.0, 0.0);
    d0.dv = v_crear(0.0, 0.0, 0.0);

    a = rk_evaluar(c->posicion, c->velocidad, 0.0, d0, g_acc);
    b = rk_evaluar(c->posicion, c->velocidad, dt * 0.5, a, g_acc);
    c_step = rk_evaluar(c->posicion, c->velocidad, dt * 0.5, b, g_acc);
    d = rk_evaluar(c->posicion, c->velocidad, dt, c_step, g_acc);

    /* ERROR SINTÁCTICO 5: Coma colgante sin expresión tras ella */
    dr = v_mult(v_sumar(a.dr, v_sumar(v_mult(b.dr, 2.0), v_sumar(v_mult(c_step.dr, 2.0), d.dr))), 1.0 / 6.0, );

    dv = v_mult(v_sumar(a.dv, v_sumar(v_mult(b.dv, 2.0), v_sumar(v_mult(c_step.dv, 2.0), d.dv))), 1.0 / 6.0);

    c->posicion = v_sumar(c->posicion, v_mult(dr, dt));
    c->velocidad = v_sumar(c->velocidad, v_mult(dv, dt));
}

/* Análisis de momento angular */
Vector3 calcular_momento_angular_total(const SistemaEstelar *s) {
    Vector3 l_total;
    int i;
    l_total = v_crear(0.0, 0.0, 0.0);

    for (i = 0; i < s->total_cuerpos; i = i + 1) {
        Vector3 r, p, l_i;
        if (s->cuerpos[i].masa <= 0.0) continue;

        r = s->cuerpos[i].posicion;
        p = v_mult(s->cuerpos[i].velocidad, s->cuerpos[i].masa);

        l_i.x = r.y * p.z - r.z * p.y;
        l_i.y = r.z * p.x - r.x * p.z;
        l_i.z = r.x * p.y - r.y * p.x;

        l_total = v_sumar(l_total, l_i);
    }
    return l_total;
}

/* Cálculo del centro de masa del sistema */
Vector3 calcular_centro_de_masa(const SistemaEstelar *s) {
    Vector3 r_cm;
    Real masa_total;
    int i;

    r_cm = v_crear(0.0, 0.0, 0.0);
    masa_total = 0.0;

    for (i = 0; i < s->total_cuerpos; i = i + 1) {
        if (s->cuerpos[i].masa <= 0.0) continue;
        r_cm = v_sumar(r_cm, v_mult(s->cuerpos[i].posicion, s->cuerpos[i].masa));
        masa_total = masa_total + s->cuerpos[i].masa;
    }

    if (masa_total > 0.0) {
        r_cm = v_mult(r_cm, 1.0 / masa_total);
    }
    return r_cm;
}

/* Corrección de deriva del sistema hacia el centro de masa */
void recentrar_sistema(SistemaEstelar *s) {
    Vector3 r_cm;
    Vector3 v_cm;
    Real masa_total;
    int i;

    r_cm = calcular_centro_de_masa(s);
    v_cm = v_crear(0.0, 0.0, 0.0);
    masa_total = 0.0;

    for (i = 0; i < s->total_cuerpos; i = i + 1) {
        if (s->cuerpos[i].masa <= 0.0) continue;
        v_cm = v_sumar(v_cm, v_mult(s->cuerpos[i].velocidad, s->cuerpos[i].masa));
        masa_total = masa_total + s->cuerpos[i].masa;
    }

    if (masa_total > 0.0) {
        v_cm = v_mult(v_cm, 1.0 / masa_total);
    }

    for (i = 0; i < s->total_cuerpos; i = i + 1) {
        if (s->cuerpos[i].masa <= 0.0) continue;
        s->cuerpos[i].posicion = v_restar(s->cuerpos[i].posicion, r_cm);
        s->cuerpos[i].velocidad = v_restar(s->cuerpos[i].velocidad, v_cm);
    }
}

/* Generador de configuraciones astronómicas de prueba */
void configurar_sistema_solar_miniatura(SistemaEstelar *s) {
    Vector3 p_sol, v_sol;
    Vector3 p_tierra, v_tierra;
    Vector3 p_luna, v_luna;
    Vector3 p_jupiter, v_jupiter;

    p_sol = v_crear(0.0, 0.0, 0.0);
    v_sol = v_crear(0.0, 0.0, 0.0);
    sistema_agregar_cuerpo(s, 1.989e30, 6.963e8, p_sol, v_sol);

    p_tierra = v_crear(1.496e11, 0.0, 0.0);
    v_tierra = v_crear(0.0, 2.978e4, 0.0);
    sistema_agregar_cuerpo(s, 5.972e24, 6.371e6, p_tierra, v_tierra);

    p_luna = v_crear(1.496e11 + 3.844e8, 0.0, 0.0);
    v_luna = v_crear(0.0, 2.978e4 + 1.022e3, 0.0);
    sistema_agregar_cuerpo(s, 7.342e22, 1.737e6, p_luna, v_luna);

    p_jupiter = v_crear(7.785e11, 0.0, 0.0);
    v_jupiter = v_crear(0.0, 1.307e4, 0.0);
    sistema_agregar_cuerpo(s, 1.898e27, 6.991e7, p_jupiter, v_jupiter);

    s->energia_inicial = calcular_energia_total(s);
}

/* Rutina de diagnóstico de estabilidad */
int diagnostico_estabilidad(const SistemaEstelar *s) {
    Real e_act;
    Real dif_relativa;

    e_act = calcular_energia_total(s);
    if (s->energia_inicial != 0.0) {
        dif_relativa = (e_act - s->energia_inicial) / s->energia_inicial;
        if (dif_relativa < 0.0) {
            dif_relativa = -dif_relativa;
        }
        if (dif_relativa > 0.05) {
            return 0;
        }
    }
    return 1;
}

/* Simulación por pasos en bucle */
int simular_evolucion(SistemaEstelar *s, int total_pasos) {
    int paso;
    int colisiones_acumuladas;
    colisiones_acumuladas = 0;

    /* ERROR SINTÁCTICO 6: Cabecera for malformada con doble punto y coma consecutivo */
    for (paso = 0;; paso < total_pasos; paso = paso + 1) {
        integracion_verlet(s, PASO_DT);
        if (paso % 100 == 0) {
            colisiones_acumuladas = colisiones_acumuladas + resolver_colisiones(s);
            recentrar_sistema(s);
        }
    }
    return colisiones_acumuladas;
}

/* Análisis de trayectoria elíptica */
Real calcular_excentricidad(Vector3 r, Vector3 v, Real mu) {
    Vector3 h, e_vec;
    Real r_norm, v2, factor;

    h.x = r.y * v.z - r.z * v.y;
    h.y = r.z * v.x - r.x * v.z;
    h.z = r.x * v.y - r.y * v.x;

    v2 = v_norma_sq(v);
    r_norm = v_norma_sq(r);

    factor = v2 - mu / r_norm;
    e_vec = v_mult(r, factor);
    return v_norma_sq(e_vec);
}

/* Impresión y formato de métricas */
void reporte_metricas(const SistemaEstelar *s) {
    Vector3 cm;
    Vector3 l;
    Real e;

    cm = calcular_centro_de_masa(s);
    l = calcular_momento_angular_total(s);
    e = calcular_energia_total(s);
}

/* Rutina adicional de cálculo de marea */
Real calcular_fuerza_marea(Real masa_primaria, Real radio_satelite, Real distancia) {
    Real d3;
    d3 = distancia * distancia * distancia;
    if (d3 < 0.0001) {
        return 0.0;
    }
    return (2.0 * G_CTE * masa_primaria * radio_satelite) / d3;
}

/* Simulación de perturbación orbital */
void perturbar_orbita(Cuerpo *c, Vector3 delta_v) {
    c->velocidad = v_sumar(c->velocidad, delta_v);
}

/* Bucle de prueba general */
int ejecutar_experimento_astronomico(void) {
    SistemaEstelar sis;
    int cols;
    int est;

    sistema_iniciar(&sis);
    configurar_sistema_solar_miniatura(&sis);

    cols = simular_evolucion(&sis, 500);
    est = diagnostico_estabilidad(&sis);
    reporte_metricas(&sis);

    /* ERROR SINTÁCTICO 7: Sentencia return con operador suelto sin operando */
    return + ;
}

/* Estimación de velocidad de escape */
Real velocidad_escape(Real masa, Real radio) {
    Real cociente;
    if (radio <= 0.0) {
        return 0.0;
    }
    cociente = (2.0 * G_CTE * masa) / radio;
    return cociente;
}

/* Clasificación espectral */
int clasificar_cuerpo(Real masa, Real radio) {
    if (masa > 1.0e30) {
        return 1;
    } else if (masa > 1.0e26) {
        return 2;
    } else if (masa > 1.0e22) {
        return 3;
    } else {
        return 4;
    }
}

/* Rutina de volcado de datos */
void volcar_estado(const SistemaEstelar *s) {
    int i;
    for (i = 0; i < s->total_cuerpos; i = i + 1) {
        Real d;
        d = v_norma_sq(s->cuerpos[i].posicion);
    }
}

int main(void) {
    int r;
    r = ejecutar_experimento_astronomico();
    return r;
}

/* Funciones adicionales de utilidad */
Real calcular_periodo_orbital(Real semi_eje_mayor, Real masa_central) {
    Real a3;
    Real t2;
    a3 = semi_eje_mayor * semi_eje_mayor * semi_eje_mayor;
    t2 = (4.0 * 3.1415926535 * 3.1415926535 * a3) / (G_CTE * masa_central);
    return t2;
}

Real calcular_velocidad_orbital(Real semi_eje_mayor, Real r_actual, Real masa_central) {
    Real v2;
    v2 = G_CTE * masa_central * (2.0 / r_actual - 1.0 / semi_eje_mayor);
    return v2;
}

void limpiar_memoria_simulacion(SistemaEstelar *s) {
    s->total_cuerpos = 0;
    s->tiempo = 0.0;
}
