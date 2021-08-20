#ifndef IMAGEN_H
#define IMAGEN_H

#include "pixel.h"
#include "endianness.h"

typedef struct imagen {
    pixel_t **lienzo;
    size_t ancho, alto;
} imagen_t;

imagen_t *imagen_leer_PPM(FILE *f);
imagen_t *imagen_leer_BMP(FILE *f);
void imagen_destruir(imagen_t *imagen);
void imagen_escribir_PPM(const imagen_t *imagen, FILE *f);
void imagen_escribir_BMP(const imagen_t *imagen, FILE *f);
imagen_t *imagen_recortar(const imagen_t *imagen, size_t x0, size_t y0, size_t ancho, size_t alto);
imagen_t *imagen_clonar(const imagen_t *imagen);
void espejar_horizontal(imagen_t *imagen);
void espejar_vertical(imagen_t *imagen);

#endif