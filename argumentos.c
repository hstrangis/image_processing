#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "filtros.h"
#include "argumentos.h"
#include "pixel.h"
#include "imagen.h"

#define MAX_FILTER 50 
#define MAX_STR 50
#define MAX_CUT 50
#define MAX_APP 200

struct filter {
	filtro_t nombre_filter;
	int parametro;
};

struct flags {
	char nombre[MAX_STR];
	funcion_flag_t funcion;
};

struct recorte {
	size_t ancho; 
	size_t alto;
	size_t x0; 
	size_t y0;
};

struct argumentos {
	FILE *f_input;
	FILE *f_output;	
	struct filter filters[MAX_FILTER];
	int n_filters;
	crop_t recorte[MAX_CUT];
	int n_cortes;
	flag_t aplicaciones[MAX_APP];
	int n_apps;
	bool in_ppm;     // Si es TRUE, entra imagen PPM
	bool out_ppm;    // Si es TRUE, sale imagen PPM
}; 
	
static int flag_recorte(int argc_i, char *argv[], argumentos_t *a);
static int flag_filtro(int argc_i, char *argv[], argumentos_t *a);
static int flag_entrada(int argc_i, char *argv[], argumentos_t *a);
static int flag_salida(int argc_i, char *argv[], argumentos_t *a);
static int flag_horizontal(int argc_i, char *argv[], argumentos_t *a);
static int flag_vertical(int argc_i, char *argv[], argumentos_t *a);

const flag_t flags[] = { 

			{ "-input" ,  flag_entrada },
			{ "-output" , flag_salida  },
			{ "-filter" , flag_filtro  },
			{ "-crop" ,   flag_recorte },
			{ "-horizontal" , flag_horizontal }, 
			{ "-vertical" ,   flag_vertical   }
};			

int leer_argumentos(int n, char *argv[], argumentos_t *a) {

	int code = 1; 
	
	if(n == 1) {
		return 0;
	}
	for(size_t i = 1; i < n; i++) {
		for(size_t j = 0; j < sizeof(flags) / sizeof(flags[i]); j++) {

			if(! strcmp(argv[i], flags[j].nombre)) {
				code = flags[j].funcion(i + 1, argv, a); 
				if(code != 0) return code;
			}	
		}
	}
	return code; 
}	

argumentos_t *crear_argumento(void) {

	argumentos_t *a = malloc(sizeof(argumentos_t));
	a->f_input = stdin;
	a->f_output = stdout;
	a->in_ppm = true;
	a->out_ppm = true;
	a->n_filters = 0;
	a->n_cortes = 0;
	a->n_apps = 0;
	return a;
}

void destruir_argumento(argumentos_t *a) {

	fclose(a->f_input);
	fclose(a->f_output);
	free(a);

}

FILE *argumento_archivo_entrada(argumentos_t *a, bool *ppm_in) {

	*ppm_in = a->in_ppm;
	FILE *fi = a->f_input;
	return fi;
}

FILE *argumento_archivo_salida(argumentos_t *a, bool *ppm_out) {

	*ppm_out = a->out_ppm;
	FILE *fo = a->f_output;
	return fo;
}

bool aplicar_flags_argumentos(argumentos_t *a, imagen_t **imagen) {

	size_t i_r = 0;  //Indice vector recortes
	size_t i_f = 0;	 //Indice vector filters

	for(size_t i = 0; i < a->n_apps; i++) {

		fprintf(stderr, "%s\n", a->aplicaciones[i].nombre);

		if(!strcmp(a->aplicaciones[i].nombre, "-crop")) {

			imagen_t *imagen_crop = imagen_recortar(*imagen, a->recorte[i_r].x0, a->recorte[i_r].y0, a->recorte[i_r].ancho, a->recorte[i_r].alto);  
			
			if(imagen_crop == NULL) {
				imagen_destruir(*imagen);
				destruir_argumento(a);
				fprintf(stderr, "Error al cortar imagen\n");
				return false;
			}
			imagen_destruir(*imagen);
			*imagen = imagen_clonar(imagen_crop);
			imagen_destruir(imagen_crop); 
			i_r++;
		}
		if(!strcmp(a->aplicaciones[i].nombre, "-filter")) {

			aplicar_filtro((*imagen)->lienzo, (*imagen)->ancho, (*imagen)->alto, a->filters[i_f].nombre_filter, a->filters[i_f].parametro);
			i_f++;
		}
		if(!strcmp(a->aplicaciones[i].nombre, "-horizontal")) {

			espejar_horizontal(*imagen);
		}	
		if(!strcmp(a->aplicaciones[i].nombre, "-vertical")) {

			espejar_vertical(*imagen);
		}	
	}
	return true;
}

static int flag_recorte(int pos, char *argv[], argumentos_t *a) {
	
	size_t i = a->n_cortes;	
	size_t ancho, alto, x0, y0;
	
	if(sscanf(argv[pos], "%ldx%ld+%ld+%ld",(long int *) &ancho,(long int*) &alto,(long int*) &x0,(long int*) &y0) != 4) {
		return 4;  		
	}
		
	a->recorte[i].ancho = ancho;
	a->recorte[i].alto = alto;
	a->recorte[i].x0 = x0;
	a->recorte[i].y0 = y0;

	a->n_cortes++;
	strcpy(a->aplicaciones[a->n_apps++].nombre, "-crop");

	return 0;
}

static int flag_filtro(int pos, char *argv[], argumentos_t *a) {


	if(argv[pos] == NULL) return 5; 

	if(! cadena_a_filtro(argv[pos], &(a->filters[a->n_filters].nombre_filter))) {
        
        return 6;  
    }
    
    filtro_t filtro = a->filters[a->n_filters].nombre_filter;

    if(filtro == FILTRO_MEZCLAR) {

    	if(argv[pos + 1] == NULL) return 6; 
    	
    	a->filters[a->n_filters].parametro = (pixel_t) pixel_desde_nombre(argv[pos + 1]);

    	if((a->filters[a->n_filters].parametro == 0))  {     

    		if(strcmp(argv[pos + 1], "black")) return 7; // Si el pixel_t guardado es 0x00 y no es black
    	}	

    	a->n_filters++;
    	strcpy(a->aplicaciones[a->n_apps++].nombre, "-filter");
    	return 0;
    }

    if(filtros[filtro].parametro == 0) {
    	a->n_filters++;
    	strcpy(a->aplicaciones[a->n_apps++].nombre, "-filter");
    	return 0;
    }

    if(argv[pos + 1] == NULL) return 7;  
	
	a->filters[a->n_filters].parametro = atoi(argv[pos + 1]);
    a->n_filters++;
    strcpy(a->aplicaciones[a->n_apps++].nombre, "-filter");
    return 0;
}


static int flag_entrada(int pos, char *argv[], argumentos_t *a) {

	char *str;
	FILE *f;

	if(argv[pos] == NULL) return 2; 

	if((str = strstr(argv[pos], ".ppm"))) {	
		f = fopen(argv[pos], "rt");
		if(f == NULL) return 2; 			
	}	
	
	else {		
		if((str = strstr(argv[pos], ".bmp"))) {               
			f = fopen(argv[pos], "rb");
			if(f == NULL) return 2;  		
			a->in_ppm = false;
		} 
	}	
	if(str == NULL) return 2; 
	a->f_input = f;
	return 0; 
}

static int flag_salida(int pos, char *argv[], argumentos_t *a) {

	char *str; 
	FILE *f;

	if(argv[pos] == NULL) return 3;

	if((str = strstr(argv[pos], ".ppm"))) {
		f = fopen(argv[pos], "wt");
		if(f == NULL) return 3; 			
	}	

	else {	
		if((str = strstr(argv[pos], ".bmp"))) {
			f = fopen(argv[pos], "wb");
			if(f == NULL) return 3;  		
			a->out_ppm = false;
		} 
	}	
	if(str == NULL) return 3;               
	a->f_output = f;
	return 0; 
}

static int flag_horizontal(int _, char ** __ , argumentos_t *a) {

	strcpy(a->aplicaciones[a->n_apps++].nombre, "-horizontal");
	return 0;
}

static int flag_vertical(int _, char ** __, argumentos_t *a) {

	strcpy(a->aplicaciones[a->n_apps++].nombre, "-vertical");
	return 0;
}





