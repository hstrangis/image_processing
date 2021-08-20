#ifndef ARGUMENTOS_H
#define ARGUMENTOS_H

#include <stdbool.h>
#include "filtros.h"

typedef struct argumentos argumentos_t;

typedef int (*funcion_flag_t)(int, char **, argumentos_t *);

typedef struct flags flag_t;

typedef struct recorte crop_t;

argumentos_t *crear_argumento(void);
void destruir_argumento(argumentos_t *a);
int leer_argumentos(int n, char *argv[], argumentos_t *a);
FILE *argumento_archivo_entrada(argumentos_t *a, bool *ppm_in);
FILE *argumento_archivo_salida(argumentos_t *a, bool *ppm_out);
bool aplicar_flags_argumentos(argumentos_t *a, imagen_t **imagen);

#endif