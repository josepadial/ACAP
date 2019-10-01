/*
	Autor: Jose Antonio Padial Molina
	Curso: 2017/2018
	Asignatura: ACAP
	Profesor: María Isabel García Arenas
*/

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cuda.h>
#include <time.h>

using namespace std;

//Función para redondear la C prima
//Utilizaremos la función ceil, que se encarga de redondear el argumento redondeado hacia arriba
void redondeo(float &numero0, float &numero1, double cifras){

	double i, j;

	i=((numero0*pow(10,cifras))-0.02)/pow(10,cifras);
	j=((numero1*pow(10,cifras))-0.02)/pow(10,cifras);

	numero0=ceil(i);
	numero1=ceil(j);
}


//Funcion de "nucleo" que calcula la suma dando a cada hebra la
//funcion de calcular la suma de los elementos de una posicion
__global__ void vecAddKernel(float *A, float *B, float *C, int n){

	int i = threadIdx.x+blockDim.x*blockIdx.x;
	if(i<n)
	  C[i] = A[i]+B[i];
}




int leeDimension (char fichero [20]){
	fstream filein1;

	filein1.open(fichero, ios::in);
		if(!filein1){
			cerr<<"No se pudo abrir el fichero1"<<endl;
			exit(1);
		}
	int filas=0;
	char caracter;
	while( filein1.get(caracter) ){
		if(caracter=='\n')
			filas++;
	}
	return filas;
}

//Función que suma en CUDA dos vectores
void vecAdd(float *hA, float *hB, float *hC, int n,  double &ncgt){
	int size = n*sizeof(float);
	float * dA, *dB, *dC;
	struct timespec cgt1,cgt2;
	int BlockDim;

	//Reservamos memoria y movemos las entradas a la memoria del device
	cudaMalloc((void **) &dA,size);
	cudaMemcpy(dA,hA,size,cudaMemcpyHostToDevice);
	cudaMalloc((void **) &dB,size);
	cudaMemcpy(dB,hB,size,cudaMemcpyHostToDevice);
	cudaMalloc((void **) &dC,size);

	if(n>1024)
		BlockDim=512;
	else
		BlockDim=ceil(n/2)+1;

	dim3 dimblock(BlockDim,1,1);
	dim3 dimgrid(ceil(n/BlockDim)+1 ,1,1);

//Iniciamos el nucleo para calcular la suma
	clock_gettime(CLOCK_REALTIME,&cgt1);
	vecAddKernel<<< dimgrid, dimblock>>>(dA, dB, dC, n);
 	clock_gettime(CLOCK_REALTIME,&cgt2);

	ncgt=(double) (cgt2.tv_sec-cgt1.tv_sec)+
	(double) ((cgt2.tv_nsec-cgt1.tv_nsec)/(1.e+9));

//Copiamos el resultado del host
	cudaMemcpy(hC,dC,size,cudaMemcpyDeviceToHost);
//Liberamos memoria
	cudaFree(dA);cudaFree(dB);cudaFree(dC);
}





int main(int argc, char **argv){

	if(argc< 4){
		cerr<<"Error al introducir argumentos, el orden de los vectores sería:\nVector1\nVector2\nOutput\n";
		exit(1);
	}
	printf("********************TARJETA***************************\n" );
	int numeroDevice;
  cudaGetDeviceCount(&numeroDevice);
  for (int i = 0; i < numeroDevice; i++) {
    cudaDeviceProp propiedades;
    cudaGetDeviceProperties(&propiedades, i);
    printf("Dispositivo Numero: %d\n", i);
    printf("  Nombre del dispositivo: %s\n", propiedades.name);
    printf("  Frecuencia Reloj (KHz): %d\n",
           propiedades.memoryClockRate);
    printf("  Memory Bus Width (bits): %d\n",
           propiedades.memoryBusWidth);
    printf("  Ancho de Banda (GB/s): %f\n\n",
           2.0*propiedades.memoryClockRate*(propiedades.memoryBusWidth/8)/1.0e6);
  }

	printf("********************TARJETA***************************\n" );
	int n=leeDimension(argv[1]);

	// cout<< endl<<" Tamanno Vectores = "<<n<<endl;

	float *h_A, *h_B, *h_C, *vector_output;

	h_A= new float[n];
	h_B=new float[n];
	h_C=new float[n];
	vector_output=new float[n];

//Leemos el fichero
	ifstream imput0 (argv[1], ifstream::in);
	ifstream imput1 (argv[2], ifstream::in);
	ifstream output (argv[3], ifstream::in);

	char c[20];
//Convertimos la cadena a un float
	for(int i=0; i<=n; i++){
		imput0 >>c;
		h_A[i]=atof(c);

	 	imput1 >>c;
		h_B[i]=atof(c);

		output >>c;
		vector_output[i]=atof(c);
	}

// almacenamiento de n elementos h_A,h_B y h_C

	double ncgt;
	vecAdd(h_A, h_B, h_C, n, ncgt);



//Mostrar vectores y resultados
 /*
	for(int i=0; i<filas; i++)
		cout<<"h_C["<<i<<"]="<<h_C[i]<<"	 	vector_output["<<i<<"]="<<vector_output[i]<<endl;
*/
for(int i= 1;i<=n;i++)
cout<<h_A[i]<<"+"<<h_B[i]<<"="<<h_C[i]<<endl;

// Comprobacion
	int linea=0;
	bool no_falla=true;

	for(int i=1; i<n && no_falla; i++){
		redondeo(h_C[i],vector_output[i],2);
		if(h_C[i]!=vector_output[i]){
			no_falla=false;
			linea=i;
		}
	}

	if(no_falla)
		cout<<"\nResultados correctos"<<endl;


	cout.precision(8);
	cout.setf(ios::fixed);


	cout<<"Tamaño del vector : "<<n<<"\nTiempo de ejecución : "<<ncgt<<endl<<endl;

}
