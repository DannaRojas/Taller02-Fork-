/**********************************************************
*            Pontificia Universidad Javeriana             *
*                         ---=---                         *
*                                                         *
* Autores: - Danna Gabriela Rojas Bernal                  *
*          - Juan David Daza Caro                         *
* Fecha: 30 Octubre de 2025                               *
* Docente: J. Corredor                                    *
* Objetivo: Aplicar los conceptos de procesos    	  *
*	    comunicación entre procesos                   *
*                                                         *
* Descripción:                                            *
***********************************************************/


//Declaracion de librerias
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int* leer_archivo(char *nombre, int n) {
    FILE *arch = fopen(nombre, "r");
    if  (arch == NULL) {
        printf("Error al abrir el archivo.\n");
        exit(1);
    } 
      

    int *arr = (int*) malloc(n * sizeof(int));//crea memoria dinamica para guardar  n numeros enteros 
    for (int i = 0; i < n; i++) {
        fscanf(arch, "%d", &arr[i]);// lee numeros del archivo (&arr[i] dirreccion de memmoria donde se va  guardar el numero)
    }
    fclose(arch);
    return arr;
}

int* leer_archivo(char *nombre, int n);
int suma_arreglo(int *arr, int n);

int main(int argc, char *argv[]) {
    if (argc != 5) {
	printf("Uso: %s N1 archivo00 N2 archivo01\n", argv[0]);
    return 0;
	}
    int N1 = atoi(argv[1]);
    int N2 = atoi(argv[3]);
    int *arr1 = leer_archivo(argv[2], N1);
    int *arr2 = leer_archivo(argv[4], N2);

        int pipefd1[2],pipefd2[2],pipefdTotal[2];
        

        if(pipe(pipefd1)==-1 || pipe(pipefd2) == -1 || pipe(pipefdTotal) == -1){
           perror("PIPE");
                exit(EXIT_FAILURE);
        }
	pid_t pid1, pid2, pidTotal;
        // proceso 1
	pid1 = fork();
        if(pid1 == 0){
	close(pipefd1[0]); // cerrar lectura
        int suma01 = suma_arreglo(arr1, N1);
        write(pipefd1[1], &suma01, sizeof(int)); // enviar suma al padre
        close(pipefd1[1]); // cerrar escritura
        free(arr1);
        free(arr2);
        exit(0);
    }else{
	close(pipefd1[1]); // cerrar escritura (el padre solo lee)
        waitpid(pid1, NULL, 0); // esperar a que el hijo termine
        int sumaA;
        read(pipefd1[0], &sumaA, sizeof(int)); // leer lo que escribió el hijo
        close(pipefd1[0]); // cerrar lectura
        printf("Suma del archivo %s: %d\n", argv[2], sumaA);
     
//proceso 2
	pid2 = fork();
        if (pid2 == 0) {
        close(pipefd2[0]);
        int suma02 = suma_arreglo(arr2, N2);
        write(pipefd2[1], &suma02, sizeof(int));
        close(pipefd2[1]);
        free(arr1);
        free(arr2);
        exit(0);
         } else {

            close(pipefd2[1]);
            waitpid(pid2, NULL, 0);
            int sumaB;
            read(pipefd2[0], &sumaB, sizeof(int));
            close(pipefd2[0]);
            printf("Suma del archivo %s: %d\n", argv[4], sumaB);

//porceos 3
            pidTotal = fork();
            if (pidTotal == 0) {
                close(pipefdTotal[0]);
                int sumaTotal = suma_arreglo(arr1, N1) + suma_arreglo(arr2, N2);
                write(pipefdTotal[1], &sumaTotal, sizeof(int));
                close(pipefdTotal[1]);
                free(arr1);
                free(arr2);
                exit(0);
            } 
            else {
                close(pipefdTotal[1]);
                waitpid(pidTotal, NULL, 0);
                int total;
                read(pipefdTotal[0], &total, sizeof(int));
                close(pipefdTotal[0]);
                printf("Suma total: %d\n", total);
            }
        }
    }


  free(arr1);
  free(arr2);
  return 0;

}
//funcion con algoritmo basico de suma de arreglos

int suma_arreglo(int *arr, int n) {
    int suma = 0;
    for (int i = 0; i < n; i++){
          suma += arr[i];
        }
    return suma;
}

