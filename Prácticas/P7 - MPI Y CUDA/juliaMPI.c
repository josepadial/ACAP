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
#include "mpi.h"
#include <math.h>

int main(int argc, char* argv[]) {

	long  tx = 256 ,ty = 256;		// tamaño en pixel de la imagen (equivale al zoom) CAMBIAR  	DEFECTO 256, 256
	float cr=0.626,ci=0.0;	// Valor de c													DEFECTO 0.285, 0.01
	float or = -1 	,oi = -1;		// Esq inferior izquierda del espacio a evaluar (encuadre) 		DEFECTO -1, -1
	float dr = 2 	,di = 2;		// Esq superior derecha del espacio a evaluar (encuadre)  		DEFECTO 1, 1

	float tamr = dr-or;
	float tami = di-oi;
	float incr = tamr/(float)tx;	// Establece la cuadrícula por parte de la x
	float inci = tami/(float)ty;	// Establece la cuadrícula por parte de la y

	float zr,zi;					// Z actual
	float zrs,zis; 					// Z siguiente
	int i,j,k,l;					// Iteradores
	unsigned char color;

	MPI_Status status;

	int id_proceso, num_procesos;	// Identificador del proceso y número de procesos de la máquina

	int tam_imagen,tam_fraccion;	// Tamaño de la imagen y de la fracción enviada a cada proceso
	int indice,indice_recibido;

	// Inicializacion de la matriz de la imagen
	char imagen [ty][tx];

	// Inicialización imagen tras la paralelización
	char imagen_paralelizada[ty][tx];

	// Tamaño de la imagen
	tam_imagen = tx*ty;

	int source;
  double start_time, end_time;
  double total_time;

	// Inicialización de MPI
	MPI_Init(&argc, &argv);
	// Inicialización del identificador de proceso y del número de procesos
	MPI_Comm_rank (MPI_COMM_WORLD, &id_proceso);
	MPI_Comm_size (MPI_COMM_WORLD, &num_procesos);

	//Tamaño de la fracción (ty de cada fracción) sin tener en cuenta el proceso 0
	tam_fraccion = ty/(num_procesos-1);

	// Inicialización de la fracción de matriz que se pasará a cada proceso
	char fraccion[tam_fraccion][tx];

	// Inicialización de la fracción que recibe el proceso 0
	char fraccion_recibida[tam_fraccion][tx];

	start_time = MPI_Wtime();

	if(id_proceso!=0)	// Todos los procesos menos el 0
	{
		// Cálculo del índice donde empieza cada fracción
		indice = tam_fraccion*(id_proceso-1);

		// Bucle para atribuir los valores a cada fracción en función de su índice
		for (i=indice; i<indice+tam_fraccion; i++)
			for (j=0; j<tx; j++)
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

				// Asignación el color
				fraccion[i][j] = color-1;
			}

		// Envia el paquete
		MPI_Send(&fraccion[indice][0],tam_fraccion*tx,MPI_CHAR,0,0,MPI_COMM_WORLD);
	}
	else	// Proceso 0
	{

		for (source=1; source < num_procesos; source++)
		{
			// Recibe el paquete
			MPI_Recv(fraccion,tam_fraccion*tx,MPI_CHAR, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);

			// Restamos 1 porque el proceso 0 no se tiene en cuenta
			indice_recibido = tam_fraccion*(status.MPI_SOURCE-1);

			// Almacenamos los valores recibidos en la matriz
			k=0;
			for (i=0; i<tam_fraccion; i++)
				for (j=0; j<tx; j++)
					imagen_paralelizada[indice_recibido+i][j] = fraccion[i][j];
		}
	}

	// Todos los procesos han realizado ya su tarea
	MPI_Barrier(MPI_COMM_WORLD);

	// Sólo el proceso 0 escribirá en el archivo .raw
	if(id_proceso==0)
	{
		//printf("Tiempo Conjunto julia: %li milisegundos\n\n",(clock()-comienzo)*1000/CLOCKS_PER_SEC);
    end_time = MPI_Wtime();


		// Paso los valores de la matriz al fichero Imagen
		FILE *fim;
		// Guarda la imagen en un fichero
		fim = fopen("juliaZ7.raw","wb");

		if (!fim)
		{
		    printf("No se puede abrir el fichero de salida.\n");
		    exit(1);
		}

		for(i=ty-1;i>=0;i--)
			fwrite(imagen_paralelizada[i],sizeof(char),tx,fim);
		fclose(fim);
    total_time = (end_time - start_time);

    fprintf(stderr, "Tiempo total del conjunto de Julia =%.16g millisegundos\n", total_time);
	}

	MPI_Finalize();

	return EXIT_SUCCESS;
}
