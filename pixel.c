#include <stdio.h>
#include <string.h>
#include <math.h>
#include "pixel.h"

#define MASK_RED    0x00FF0000  // 0000 0000 1111 1111 0000 0000 0000 0000     
#define MASK_GREEN  0x0000FF00  // 0000 0000 0000 0000 1111 1111 0000 0000
#define MASK_BLUE   0x000000FF  // 0000 0000 0000 0000 0000 0000 1111 1111

#define SHIFT_RED   16
#define SHIFT_GREEN 8 

struct color {

    const char *nombre;
    const pixel_t valor;
};

const color_t color_rgb[] = {

        { "black",   0x000000 },
        { "white",   0xFFFFFF },
        { "red",     0xFF0000 },
        { "lime",    0x00FF00 },
        { "blue",    0x0000FF },
        { "yellow",  0xFFFF00 },
        { "cyan",    0x00FFFF },
        { "magenta", 0xFF00FF },
        { "silver",  0xC0C0C0 },    
        { "gray",    0x808080 },
        { "maroon",  0x800000 },
        { "olive",   0x808000 },
        { "green",   0x008000 },
        { "purple",  0x800080 },
        { "teal",    0x008080 },
        { "navy",    0x000080 }
};

pixel_t pixel_desde_rgb(componente_t rojo, componente_t verde, componente_t azul) {

    return ((rojo << SHIFT_RED) | (verde << SHIFT_GREEN) | azul); 
}   

componente_t pixel_get_rojo(pixel_t p) {

    return ((p & MASK_RED) >> SHIFT_RED); 
}

componente_t pixel_get_verde(pixel_t p) {

    return ((p & MASK_GREEN) >> SHIFT_GREEN);
}

componente_t pixel_get_azul(pixel_t p) {
    
    return (p & MASK_BLUE); 
}   

void pixel_a_hsv(pixel_t p, short *h, float *s, float *v) {
    float rp = pixel_get_rojo(p) / 255.0;
    float vp = pixel_get_verde(p) / 255.0;
    float ap = pixel_get_azul(p) / 255.0;

    float cmax = rp > vp ? rp : vp;
    cmax = cmax > ap ? cmax : ap;

    float cmin = rp < vp ? rp : vp;
    cmin = cmin < ap ? cmin : ap;

    float delta = cmax - cmin;

    if(delta == 0)
        *h = 0;
    else if(cmax == rp)
        *h = (int)(60 * (vp - ap) / delta + 360.5) % 360;
    else if(cmax == vp)
        *h = (int)(60 * (ap - rp) / delta + 120.5) % 360;
    else
        *h = (int)(60 * (rp - vp) / delta + 240.5) % 360;

    *s = cmax == 0 ? 0 : (delta / cmax);
    *v = cmax;
}

pixel_t pixel_desde_hsv(short h, float s, float v) {
    float c = s * v;
    float x = c * (1 - fabs(fmodf(h / 60.0, 2) - 1));
    float m = v - c;

    float r = 0, v_ = 0, a = 0;

    if(h < 60)
        r = c, v_ = x;
    else if(h < 120)
        v_ = c, r = x;
    else if(h < 180)
        v_ = c, a = x;
    else if(h < 240)
        a = c, v_ = x;
    else if(h < 300)
        a = c, r = x;
    else
        r = c, a = x;

    return pixel_desde_rgb((r + m) * 255, (v_ + m) * 255, (a + m) * 255);
}

pixel_t pixel_desde_nombre(const char *nombre) {

    for(size_t i = 0; i < (sizeof(color_rgb) / sizeof(color_t)); i++) {
        
        if( !strcmp(color_rgb[i].nombre, nombre) ) {
            return color_rgb[i].valor;
        }
    }
    return color_rgb[black].valor;  
}