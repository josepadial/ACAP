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
#include <sys/time.h>
#include <cuda.h>

#define TX 512
#define TY 512

__device__ short CalcJulia(int x_ind, int y_ind){

	float cr=0.626;		// Valor de c
	float ci=0.0;
	float oror = -1.0;
	float oi = -1.0;
	float dr=1,di=1;		// Esq superior derecha del espacio a evaluar (encuadre)

	float tamr=dr-oror;
	float tami=di-oi;
	float incr=tamr/(float)TX;
	float inci=tami/(float)TY;

	float zr,zi;
	float zrs,zis;
	short color;

	zi=(float)oi+(float)y_ind*incr;
	zr=(float)oror+(float)x_ind*inci;
	zrs=zis=(float)0; // semilla Z0
	color=0;	    // colorea y limita las iteraciones de puntos del conjunto

	while (zrs + zis < (float) 4 && color < 256) {
		zrs = zr * zr;
	zis = zi * zi;
	float zrsaux = zr;

		zr=(-(float)7*zr*pow(zi,6)) + ((float)35*pow(zr,3)*pow(zi,4)) - ((float)21*pow(zr,5)*pow(zi,2))+pow(zr,7) + cr;
		zi=-pow(zi,7)+((float)21*pow(zi,5)*pow(zrsaux,2))-((float)35*pow(zrsaux,4)*pow(zi,3)) + ((float)7*pow(zrsaux,6)*zi)  + ci;
		color++;
}

	return color;
}

__global__ void JuliaKernel(char *imagen){
	int i = threadIdx.x + blockIdx.x*blockDim.x; //
	int j = blockIdx.y;

	if (i < TX && j < TY) imagen[i + j*gridDim.x*blockDim.x] = CalcJulia(i, j); // Escribo en la columna i y fila j
}

int main(int argc, char* argv[]) {

	int i;

	struct timeval start, end;
	float gpu_time; // GPU time wm(with memory)

	gettimeofday(&start, NULL);

	char *im = (char*) malloc(TX*TY*sizeof(char));
	char *im_d; //Imagen en la memoria de la GPU

	cudaMalloc((void **) &im_d, TX*TY*sizeof(char));


	dim3 DimGrid(TX/512,TY,1); // 512 is the maximum number of threads per block.
	dim3 DimBlock(512,1,1); //Number of threads per block

	JuliaKernel<<<DimGrid,DimBlock>>>(im_d);
	cudaThreadSynchronize();

	cudaMemcpy(im, im_d, TX*TY*sizeof(char), cudaMemcpyDeviceToHost);

	gettimeofday(&end, NULL);

	gpu_time = (end.tv_sec - start.tv_sec) * 1000.0;      // sec to ms
     	gpu_time += (end.tv_usec - start.tv_usec) / 1000.0;   // us to ms

	printf("%d\t%f\n", TX, gpu_time);

	 FILE *fim;
	 fim=fopen("julia.raw","wb");
	 if (!fim)
	 {
	     printf("No se puede abrir el fichero de salida.\n");
	     exit(1);
	 }
	 for(i=0;i < TY;++i){
	 	fwrite(&(im[i*TX]),sizeof(char),TX,fim);
	 }
	 fclose(fim);

	cudaFree(im_d);
	free(im);

	return EXIT_SUCCESS;
}
