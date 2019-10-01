/*
    @autor Jose Antonio Padial Molina
    Archivo que muestra las caracteristicas de nuestros Devices
*/
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
int main() {
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
}
