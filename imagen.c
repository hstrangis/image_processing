#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include "imagen.h"

#define MAX_AUX 1000
#define MAX_ALTO 2000
#define MAX_ANCHO 2000

/* Definicion de la estructura dinamica que contiene los datos int32_t del archivo BMP en imagen_escribir_BMP */
typedef struct formato_32bits {

        char *nombre;
        int32_t valor;
    } bmp_32_t;

enum datos_32 {

        tam_archivo, reservado, offset, tam, ancho, alto, compresion, tam_imagen, res_x, res_y, tablas_color, color_imp
};    

static int leer_entero(FILE *f) {

    size_t i = 0;
    char aux[MAX_AUX];
    char car = 0;
    
    while(car != EOF) {

        car = fgetc(f);
        if(car == '#') { 
            while(car != '\n') {
                car = fgetc(f);
            }   
            continue;
        }
        if(isdigit(car)) {
            while(!isspace(car)) {
                aux[i] = car;
                i++;
                car = fgetc(f);
            }
            aux[i] = '\0';  
            return atoi(aux);       
        }
    }   
    return -1;
}

void imagen_destruir(imagen_t *imagen) {
    
    for(size_t i = 0; i < imagen->alto; i++) {
        free(imagen->lienzo[i]);
    }
    free(imagen->lienzo);
    free(imagen);    
}

static imagen_t *imagen_crear(size_t ancho, size_t alto) {

    imagen_t *imagen;
    imagen = malloc(sizeof(imagen_t));
    if(imagen == NULL) {
        return NULL;
    }
    imagen->ancho = ancho;
    imagen->alto = alto;
    imagen->lienzo = malloc(alto * sizeof(pixel_t *));
    if(imagen->lienzo == NULL) {
        free(imagen);
        return NULL;
    }
    for(size_t i = 0; i < alto; i++) {
        imagen->lienzo[i] = malloc(ancho * sizeof(pixel_t));
        
        if(imagen->lienzo[i] == NULL) {
            imagen->alto = i;
            imagen_destruir(imagen);
            return NULL;
        }       
    }
    return imagen;
}

imagen_t *imagen_leer_PPM(FILE *f) {

    char aux[10];
    fgets(aux, 10, f);
    if(strcmp(aux, "P3\n"))
        return NULL;

    size_t ancho = leer_entero(f);
    size_t alto = leer_entero(f);
    int max = leer_entero(f);

    if(max != 255)
        return NULL;

    if(ancho > MAX_ANCHO || alto > MAX_ALTO)
        return NULL;

    imagen_t *imagen = imagen_crear(ancho, alto);

    for(int y = 0; y < alto; y++) {
        for(int x = 0; x < ancho; x++) {
            componente_t rojo = leer_entero(f);
            componente_t verde = leer_entero(f);
            componente_t azul = leer_entero(f);
            imagen->lienzo[y][x] = pixel_desde_rgb(rojo, verde, azul);
        }
    }        
    return imagen;
}

void imagen_escribir_PPM(const imagen_t *imagen, FILE *f) {

    fprintf(f, "P3\n");
    fprintf(f, "%zd %zd 255\n", imagen->ancho, imagen->alto);

    for(int y = 0; y < imagen->alto; y++)
        for(int x = 0; x < imagen->ancho; x++) {
            pixel_t pixel = imagen->lienzo[y][x];
            fprintf(f, "%d %d %d\n", pixel_get_rojo(pixel), pixel_get_verde(pixel), pixel_get_azul(pixel));
        }
}

imagen_t *imagen_recortar(const imagen_t *imagen, size_t x0, size_t y0, size_t ancho, size_t alto) {

    imagen_t * imagen_recortada;
    size_t borde_x = x0 + ancho;
    size_t borde_y = y0 + alto;

    if(x0 >= imagen->ancho || y0 >= imagen->alto) {
        return NULL;
    }
    if(borde_x > imagen->ancho) {
        ancho = (imagen->ancho) - x0;
    }    
    if(borde_y > imagen->alto) {    
        alto = (imagen->alto) - y0;
    }
    imagen_recortada = imagen_crear(ancho, alto);
    if(imagen_recortada == NULL) {
        return NULL;
    }
    for(size_t y = 0; y < alto; y++) { 
        for(size_t x = 0; x < ancho; x++) {
            imagen_recortada->lienzo[y][x] = imagen->lienzo[y + y0][x + x0];
        }
    }
    return imagen_recortada;
}

imagen_t *imagen_clonar(const imagen_t *imagen) {

    return imagen_recortar(imagen, 0, 0, imagen->ancho, imagen->alto);
}

imagen_t *imagen_leer_BMP(FILE *f) {

    char bm[2];
    int32_t tam_archivo;
    int32_t datos_32[MAX_AUX];
    int32_t ancho, alto;
    int16_t datos_16[MAX_AUX];
    componente_t vec_bgr[3];
    componente_t ceros_scan[MAX_AUX];
    size_t add_fil = 0;
    
    if(fread(&bm, sizeof(char), 2, f) != 2) return NULL;
    
    if(! fread(&tam_archivo, sizeof(int32_t), 1, f)) return NULL;

    if(fread(&datos_32, sizeof(int32_t), 3, f) != 3) return NULL;
    
    if(! fread(&ancho, sizeof(int32_t), 1, f)) return NULL;
    if(! fread(&alto, sizeof(int32_t), 1, f)) return NULL;

    if(ancho % 4 != 0) {
        add_fil = 4 - ((ancho * 3) % 4);
    }

    size_t scanline = (ancho * 3) + add_fil;

    imagen_t *imagen = imagen_crear(ancho, alto);

    if(fread(&datos_16, sizeof(int16_t), 2, f) != 2) {
        imagen_destruir(imagen);
        return NULL;  
    } 

    if(fread(&datos_32, sizeof(int32_t), 6, f) != 6) {
        imagen_destruir(imagen);
        return NULL;
    }    

    for(size_t y = alto; y > 0; y--) {     
        for(size_t x = 0; x < ancho; x++) {

            if(fread(vec_bgr, sizeof(componente_t), 3, f) != 3) {
                imagen_destruir(imagen);
                return NULL;
            }    
            pixel_t pixel = pixel_desde_rgb(vec_bgr[2], vec_bgr[1], vec_bgr[0]);
            imagen->lienzo[y - 1][x] = pixel;                    
        }
        if(add_fil != 0) {
            size_t scans = scanline - (imagen->ancho * 3);
            if(fread(ceros_scan, sizeof(componente_t), scans, f) != scans) { 
                imagen_destruir(imagen);
                return NULL;
            }    
        }     
    }   
    return imagen;
}

void imagen_escribir_BMP(const imagen_t *imagen, FILE *f) {
    
    char tipo[] = "BM";
    size_t add_fil = 0;
    int16_t planos = 1;
    int16_t bits_color = 24; 
    
    if(imagen->ancho % 4 != 0) {
        add_fil = 4 - ((imagen->ancho * 3) % 4);
    }

    size_t scanline = (imagen->ancho * 3) + add_fil;

    bmp_32_t datos_32[] = {

                        { "tam_archivo", 14 + 40 + (imagen->alto * scanline) },
                        { "reservado",                0                      },
                        { "offset",                  54                      },                        
                        { "tam",                     40                      },
                        { "ancho",              imagen->ancho                },
                        { "alto",               imagen->alto                 },
                        { "compresion",               0                      },
                        { "tam_imagen",               0                      },
                        { "res_x",                    0                      },
                        { "res_y",                    0                      },
                        { "tablas_color",             0                      },
                        { "color_imp",                0                      },

    }; 

    fwrite(tipo, sizeof(char), 2, f);

    for(size_t i = 0; i < compresion; i++) {
        escribir_int32_little_endian(f, datos_32[i].valor);
    }
    escribir_int16_little_endian(f, planos);
    escribir_int16_little_endian(f, bits_color);
    for(size_t i = compresion; i < color_imp + 1; i++) {
        escribir_int32_little_endian(f, datos_32[i].valor);
    }
    
    for(size_t y = imagen->alto; y > 0; y--) {     
        for(size_t x = 0; x < imagen->ancho; x++)  {

            pixel_t pixel = imagen->lienzo[y - 1][x];
            componente_t vec_bgr[] = { pixel_get_azul(pixel), pixel_get_verde(pixel), pixel_get_rojo(pixel) }; 
            fwrite(vec_bgr, sizeof(componente_t), 3, f);
        }
        if(add_fil != 0) {
            fwrite("0", sizeof(componente_t), scanline - (imagen->ancho * 3), f);        
        }
    }   
}

void espejar_horizontal(imagen_t *imagen) {

    pixel_t aux = 0;
    for(size_t i = 0; i < imagen->alto ; i++) {
        for(size_t j = 0; j < imagen->ancho / 2; j++) {   
            aux = imagen->lienzo[i][j];
            imagen->lienzo[i][j] = imagen->lienzo[i][imagen->ancho - j - 1];
            imagen->lienzo[i][imagen->ancho - j - 1] = aux;    
        }
    }
}

void espejar_vertical(imagen_t *imagen) {

    pixel_t aux = 0;
    for(size_t j = 0; j < imagen->ancho ; j++) {
        for(size_t i = 0; i < imagen->alto / 2; i++) {   
            aux = imagen->lienzo[i][j];
            imagen->lienzo[i][j] = imagen->lienzo[imagen->alto - i - 1][j];
            imagen->lienzo[imagen->alto - i - 1][j] = aux;    
        }
    }

}


