/*
	Autor: Jose Antonio Padial Molina
	Curso: 2017/2018
	Asignatura: ACAP
	Profesor: María Isabel García Arenas
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

int main(int argc, char* argv[]) {

	long tx=512,ty=512;		// tamaño en pixel de la imagen (equivale al zoom)
	float cr=0.626,ci=0.0;		// Valor de c
	float or=-1,oi=-1;		// Esq inferior izquierda del espacio a evaluar (encuadre)
	float dr=1,di=1;		// Esq superior derecha del espacio a evaluar (encuadre)

	float tamr=dr-or;
	float tami=di-oi;
	float incr=tamr/(float)tx;
	float inci=tami/(float)ty;

	float zr,zi;
	float zrs,zis;
	int i,j;
	unsigned char color;
	double secs;

	// inicializacion estatica de la imagen
	char im[ty][tx];

	clock_t c1;
	clock_t comienzo=clock();
	for (j=0;j<ty;j++)
	     for (i=0;i<tx;i++)
	     {
				 zi    = (float)oi+(float)i*incr;
	 				zr    = (float)or+(float)j*inci;
	         zrs = zis = (float) 0;
	   			color = 0;
	   			while (zrs + zis < (float) 4 && color < 256) {
	   				zrs = zr * zr;
	   			zis = zi * zi;
	   			float zrsaux = zr;

	   	      zr=(-(float)7*zr*pow(zi,6)) + ((float)35*pow(zr,3)*pow(zi,4)) - ((float)21*pow(zr,5)*pow(zi,2))+pow(zr,7) + cr;
	   	      zi=-pow(zi,7)+((float)21*pow(zi,5)*pow(zrsaux,2))-((float)35*pow(zrsaux,4)*pow(zi,3)) + ((float)7*pow(zrsaux,6)*zi)  + ci;
	   		    color++;
		  }
		  im[j][i]=color-1; // asigno el color
	     }
		c1=clock();
	 printf("Tiempo Conjunto Julia: %.16g milisegundos\n",(double)(c1-comienzo)/CLOCKS_PER_SEC);
	 // paso los valores de la matriz al fichero Imagen
	 FILE *fim;
	 fim=fopen("julia.raw","wb");
	 if (!fim)
	 {
	     printf("No se puede abrir el fichero de salida.\n");
	     exit(1);
	 }
	 for(i=ty-1;i>=0;i--)
	 	fwrite(im[i],sizeof(char),tx,fim);
	 fclose(fim);

	return EXIT_SUCCESS;
}
