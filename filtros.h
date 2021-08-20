#ifndef FILTROS_H
#define FILTROS_H

#include <stdio.h>
#include <stdbool.h>
#include "pixel.h"
#include "imagen.h"

typedef enum funciones {

    FILTRO_INVERTIR, FILTRO_SATURAR, FILTRO_CAMBIAR_GAMA, FILTRO_CAMBIAR_BRILLO, FILTRO_CAMBIAR_CONTRASTE, 
    FILTRO_MEZCLAR, FILTRO_SEPIA, FILTRO_MONOCROMO, FILTRO_TOASTER, FILTRO_VALENCIA

} filtro_t;

typedef pixel_t (*funcion_t)(pixel_t, int);


struct filtro {

    const char *nombre;
    const funcion_t funcion;
    const char parametro;
};

pixel_t filtro_valencia(pixel_t p, int _);
pixel_t filtro_toaster(pixel_t p, int _);
pixel_t filtro_monocromo(pixel_t p, int porcentaje_umbral);
pixel_t filtro_sepia(pixel_t p, int _);
pixel_t filtro_mezclar(pixel_t p, int color);
pixel_t filtro_cambiar_contraste(pixel_t p, int porcentaje);
pixel_t filtro_cambiar_brillo(pixel_t p, int porcentaje);
pixel_t filtro_cambiar_gama(pixel_t p, int desplazamiento);
pixel_t filtro_saturar(pixel_t p, int porcentaje);
pixel_t filtro_invertir(pixel_t p, int _);
bool cadena_a_filtro(const char *nombre, filtro_t *filtro);
int numero_de_parametros(filtro_t filtro);
void aplicar_filtro(pixel_t **imagen, size_t ancho, size_t alto, filtro_t filtro, int parametro);

static const struct filtro filtros[] = {                                                                              

    { "invertir",  filtro_invertir,          0 },
    { "saturar",   filtro_saturar,           1 },
    { "gama",      filtro_cambiar_gama,      1 },
    { "brillo",    filtro_cambiar_brillo,    1 },
    { "contraste", filtro_cambiar_contraste, 1 },
    { "mezclar",   filtro_mezclar,           1 },
    { "sepia",     filtro_sepia,             0 },
    { "monocromo", filtro_monocromo,         1 },
    { "toaster",   filtro_toaster,           0 },
    { "valencia",  filtro_valencia,          0 }

};

#endif