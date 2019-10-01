/*
	Autor: Jose Antonio Padial Molina
	Curso: 2017/2018
	Asignatura: ACAP
	Profesor: María Isabel García Arenas
*/

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <cmath>
using namespace std;


void redondeo(float &n0, float &n1, int cifras){
	n0=ceil((n0*pow(10,cifras))-0.02)/pow(10,cifras);
	n1=ceil((n1*pow(10,cifras))-0.02)/pow(10,cifras);
}


int leeDimension (char fichero [20]){
	fstream filein1;

	filein1.open(fichero, ios::in);
		if(!filein1){
			cerr<<"No se pudo abrir el fichero1"<<endl;
			exit(1);
		}
	int Filas=0;
	char c;
	while( filein1.get(c) ){
		if(c=='\n')
			Filas++;
	}
	return Filas;
}


int main(int argc, char **argv){

	if(argc< 4){
		cerr<<"Error al introducir argumentos, el orden de los vectores sería:\nVector1\nVector2\nOutput\n";
		exit(1);
	}

	struct timespec cgt1,cgt2;

	int filas=leeDimension(argv[1]);

	float *vector1, *vector2, *vector_final, *vector_output;

	vector1=new float[filas];
	vector2=new float[filas];
	vector_final=new float[filas];
	vector_output=new float[filas];


	ifstream imput0 (argv[1], ifstream::in);
	ifstream imput1 (argv[2], ifstream::in);
	ifstream output (argv[3], ifstream::in);

	char c[20];

	for(int i=0; i<=filas; i++){
		imput0 >>c;
		vector1[i]=atof(c);

	 	imput1 >>c;
		vector2[i]=atof(c);

		output >>c;
		vector_output[i]=atof(c);
	}


// medir tiempo de calculo
	clock_gettime(CLOCK_REALTIME,&cgt1);
	for(int i=1; i<=filas; i++)
		vector_final[i]=vector1[i]+vector2[i];
	clock_gettime(CLOCK_REALTIME,&cgt2);
// medir tiempo de calculo


	double ncgt=(double) (cgt2.tv_sec-cgt1.tv_sec)+(double) ((cgt2.tv_nsec-cgt1.tv_nsec)/(1.e+9));



	//Mostrar vectores y resultados
 	/*
	for(int i=0; i<filas; i++)
		cout<<"vector_final["<<i<<"]="<<vector_final[i]<<"\tvector_output["<<i<<"]="<<vector_output[i]<<endl;
	*/

	// Comprobacion
	int linea=0;
	bool no_falla=true;
	for(int i=1; i<=filas; i++)
	cout<<vector1[i]<<"+"<<vector2[i]<<"="<<vector_final[i]<<endl;
	for(int i=1; i<=filas && no_falla; i++){
		redondeo(vector_final[i],vector_output[i],2);
		if(vector_final[i]!=vector_output[i]){
			no_falla=false;
			linea=i;
		}
	}

	if(no_falla)
		cout<<"\nResultados correctos"<<endl;
	else
		cout<<"Falla en la linea: "<<linea<<endl;

	cout.precision(8);
	cout.setf(ios::fixed);

	//Muestro resultado de la ejecucion
	cout<<"Tamaño del vector : "<<filas<<"\nTiempo de ejecución : "<<ncgt<<endl<<endl;

	// cout<<filas<<"\t"<<ncgt<<endl; //Para sacar tiempos
}
