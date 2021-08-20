#include <string.h>
#include "pixel.h"
#include "filtros.h"

pixel_t filtro_invertir(pixel_t p, int _) {
    return pixel_desde_rgb(255 - pixel_get_rojo(p), 255 - pixel_get_verde(p), 255 - pixel_get_azul(p));
}

pixel_t filtro_saturar(pixel_t p, int porcentaje) {
    short h;
    float s, v;
    pixel_a_hsv(p, &h, &s, &v);

    s *= (1 + porcentaje / 100.0);

    if(s < 0)
        s = 0;
    else if(s > 1)
        s = 1;

    return pixel_desde_hsv(h, s, v);
}

pixel_t filtro_cambiar_gama(pixel_t p, int desplazamiento) {
    short h;
    float s, v;
    pixel_a_hsv(p, &h, &s, &v);

    h = (h + desplazamiento) % 360;

    return pixel_desde_hsv(h, s, v);
}

pixel_t filtro_cambiar_brillo(pixel_t p, int porcentaje) {
    short h;
    float s, v;
    pixel_a_hsv(p, &h, &s, &v);

    v += porcentaje / 100.;

    if(v < 0)
        v = 0;
    else if(v > 1)
        v = 1;

    return pixel_desde_hsv(h, s, v);
}

pixel_t filtro_cambiar_contraste(pixel_t p, int porcentaje) {
    short h;
    float s, v;
    pixel_a_hsv(p, &h, &s, &v);

    v *= 1 + porcentaje / 100.;

    if(v < 0)
        v = 0;
    else if(v > 1)
        v = 1;

    return pixel_desde_hsv(h, s, v);
}

pixel_t filtro_mezclar(pixel_t p, int color) {
    pixel_t c = color;

    return pixel_desde_rgb(
        (pixel_get_rojo(p) + pixel_get_rojo(c)) / 2,
        (pixel_get_verde(p) + pixel_get_verde(c)) / 2,
        (pixel_get_azul(p) + pixel_get_azul(c)) / 2);
}

pixel_t filtro_sepia(pixel_t p, int _) {
    componente_t r = pixel_get_rojo(p);
    componente_t v = pixel_get_verde(p);
    componente_t a = pixel_get_azul(p);

    int rn = r * 0.393 + v * 0.769 + a * 0.189;
    int vn = r * 0.349 + v * 0.686 + a * 0.168;
    int an = r * 0.272 + v * 0.534 + a * 0.131;

    if(rn > 255) rn = 255;
    if(vn > 255) vn = 255;
    if(an > 255) an = 255;

    return pixel_desde_rgb(rn, vn, an);
}

pixel_t filtro_monocromo(pixel_t p, int porcentaje_umbral) {
    short h;
    float s, v;
    pixel_a_hsv(p, &h, &s, &v);

    return (v > porcentaje_umbral / 100.) ? pixel_desde_rgb(255, 255, 255) : pixel_desde_rgb(0, 0, 0);
}

pixel_t filtro_toaster(pixel_t p, int _) {
    p = filtro_mezclar(p, pixel_desde_rgb(51, 0, 0));

    short h;
    float s, v;
    pixel_a_hsv(p, &h, &s, &v);

    v = v * 1.5 + 0.2;
    s *= 0.8;
    h = (h + 20) % 360;

    if(v > 1)
        v = 1;

    return pixel_desde_hsv(h, s, v);
}

pixel_t filtro_valencia(pixel_t p, int _) {
    float r = pixel_get_rojo(p) / 100.;
    float ve = pixel_get_verde(p) / 100.;
    float a = pixel_get_azul(p) / 100.;

    r = 0.23 + r - 2 * 0.23 * r;
    ve = 0.01 + ve - 2 * 0.01 * ve;
    a = 0.22 + a - 2 * 0.22 * a;

    if(r > 1) r = 1;
    if(ve > 1) ve = 1;
    if(a > 1) a = 1;

    if(r < 0) r = 0;
    if(ve < 0) ve = 0;
    if(a < 0) a = 0;

    p = filtro_mezclar(p, pixel_desde_rgb(r * 255, ve * 255, a * 255));

    short h;
    float s, v;
    pixel_a_hsv(p, &h, &s, &v);

    v = v * 1.08 - 0.08;
    if(v > 1)
        v = 1;
    if(v < 0)
        v = 0;

    p = pixel_desde_hsv(h, s, v);
    pixel_t c = filtro_sepia(p, 0);

    return pixel_desde_rgb(
        pixel_get_rojo(p) * 0.92 + pixel_get_rojo(c) * 0.08,
        pixel_get_verde(p) * 0.92 + pixel_get_verde(c) * 0.08,
        pixel_get_azul(p) * 0.92 + pixel_get_azul(c) * 0.08);
}

bool cadena_a_filtro(const char *nombre, filtro_t *filtro) {

    for(size_t i = 0; i < sizeof(filtros)/sizeof(filtros[0]); i++) {

        if( !strcmp(nombre, filtros[i].nombre) ) {    
            *filtro = i;
            return true;    
        }
    }
    return false;
}

int numero_de_parametros(filtro_t filtro) {
    
    return filtros[filtro].parametro;
}

void aplicar_filtro(pixel_t **imagen, size_t ancho, size_t alto, filtro_t filtro, int parametro) {

    for(size_t y = 0; y < alto; y++) {
        for(size_t x = 0; x < ancho; x++) {
            pixel_t pixel = imagen[y][x];
            pixel_t pixel_fx = filtros[filtro].funcion(pixel, parametro); 
            imagen[y][x] = pixel_fx;
        }
    }
}
