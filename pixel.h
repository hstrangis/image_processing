#ifndef PIXEL_H
#define PIXEL_H

#include <stdint.h>

typedef uint8_t componente_t;  

typedef uint32_t pixel_t;

typedef struct color color_t;

enum colores { 

    black, white, red, lime, blue, yellow, cyan, magenta, silver, gray, maroon, olive, green, purple, teal, navy
};

pixel_t pixel_desde_rgb(componente_t rojo, componente_t verde, componente_t azul);
pixel_t pixel_desde_hsv(short h, float s, float v);
componente_t pixel_get_rojo(pixel_t p);
componente_t pixel_get_verde(pixel_t p); 
componente_t pixel_get_azul(pixel_t p);
void pixel_a_hsv(pixel_t p, short *h, float *s, float *v);
pixel_t pixel_desde_nombre(const char *nombre);

#endif