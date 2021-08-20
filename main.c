#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "imagen.h"
#include "argumentos.h"

int main(int argc, char *argv[]) {

	argumentos_t *a = crear_argumento();
	imagen_t *imagen;

	int code = leer_argumentos(argc, argv, a);

	switch(code) {

		case 0: break;
		
		case 1: fprintf(stderr, "Argumentos invalidos\n");
				destruir_argumento(a);
				return 1;

		case 2: fprintf(stderr, "Archivo de entrada invalido\n");
				destruir_argumento(a);
				return 1;

		case 3: fprintf(stderr, "Archivo de salida invalido\n");
				destruir_argumento(a);
				return 1;

		case 4: fprintf(stderr, "Parametros de corte invalidos\n");
				destruir_argumento(a);
				return 1;

		case 5: fprintf(stderr, "Falta nombre de filtro\n");
				destruir_argumento(a);
				return 1;

		case 6: fprintf(stderr, "Falta parametro de filtro\n");
				destruir_argumento(a);
				return 1;

		case 7: fprintf(stderr, "Parametro de filtro invalido\n");
				destruir_argumento(a);
				return 1;

		default: destruir_argumento(a);	 
				 return 1;		
	}

	bool ppm_in;
	bool ppm_out;

	FILE *fi = argumento_archivo_entrada(a, &ppm_in);
	FILE *fo = argumento_archivo_salida(a, &ppm_out);

	if(ppm_in) {
		imagen = imagen_leer_PPM(fi);
		if(imagen == NULL) {
			destruir_argumento(a);
			fprintf(stderr, "Error al procesar la imagen\n");
			return 1;
		}
	}
	else {
		imagen = imagen_leer_BMP(fi);
		if(imagen == NULL) {
			destruir_argumento(a);
			fprintf(stderr, "Error al procesar la imagen\n");
			return 1;
		}
	}

	if(! aplicar_flags_argumentos(a, &imagen)) {
		imagen_destruir(imagen);
		destruir_argumento(a);
		fprintf(stderr, "Error al cortar imagen\n");
		return 1;
	}	
	
	if(ppm_out) {
		imagen_escribir_PPM(imagen, fo);
	}
	else {
		imagen_escribir_BMP(imagen, fo);
	}

	destruir_argumento(a);
	imagen_destruir(imagen);
	fprintf(stderr, "La operacion se realizo satisfactoriamente\n");

	return 0;
}	