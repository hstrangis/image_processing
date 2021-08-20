CFLAGS = -Wall -Werror -pedantic -std=c99
LFLAGS = -lm  
DEBUG = -g

all: tp1

tp1: main.o imagen.o pixel.o endianness.o filtros.o argumentos.o     
	gcc main.o imagen.o pixel.o endianness.o argumentos.o filtros.o -o tp1 $(CFLAGS) $(LFLAGS)

endianness.o: endianness.c endianness.h
	gcc -c endianness.c $(CFLAGS)	 

filtros.o: filtros.c filtros.h pixel.h imagen.h
	gcc -c filtros.c $(CFLAGS)

argumentos.o: argumentos.c argumentos.h filtros.h
	gcc -c argumentos.c $(CFLAGS)

imagen.o: imagen.c imagen.h pixel.h 
	gcc -c imagen.c $(CFLAGS)

pixel.o: pixel.c pixel.h 
	gcc -c pixel.c $(CFLAGS)	 

main.o: main.c imagen.h pixel.h argumentos.h 
	gcc -c main.c $(CFLAGS)

clean:
	rm *.o
	rm tp1
