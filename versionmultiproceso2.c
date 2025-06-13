// Se incluyen las librerías necesarias para manejar entrada/salida, procesos y tiempo
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <time.h>

// Se definen las constantes principales del restaurante
#define numMesas 30000            // Total de mesas que se deben atender
#define numMeseros 4              // Número de meseros disponibles
#define mesaSala 15               // Cantidad de mesas por sala

// Nombres que tendrán los meseros (solo para mostrar en pantalla)
const char* nombresMeseros[] = {"Carlos", "Luis", "María", "Ana"};

// Estructura para compartir datos entre procesos (cantidad de salas armadas)
struct DatosCompartidos {
    int salasArmadas;
};

// Función para calcular cuánto tiempo ha pasado entre dos momentos
double tiempoTranscurrido(struct timespec inicio, struct timespec fin) {
    return (fin.tv_sec - inicio.tv_sec) + (fin.tv_nsec - inicio.tv_nsec) / 1e9;
}

// Esta función arma una sala cada vez que se alcanzan 15 mesas
void armarSala(int* contadorMesas, int* contadorSalas) {
    (*contadorMesas)++;
    if (*contadorMesas == mesaSala) {
        (*contadorSalas)++;
        *contadorMesas = 0;
    }
}

int main() {
    // Se obtiene el tiempo de inicio de la jornada
    struct timespec tiempoInicioGlobal, tiempoFinGlobal;
    clock_gettime(CLOCK_MONOTONIC, &tiempoInicioGlobal);

    // Se reserva memoria compartida para que los procesos puedan sumar las salas
    struct DatosCompartidos* datos = mmap(NULL, sizeof(struct DatosCompartidos),
                                    PROT_READ | PROT_WRITE,
                                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    datos->salasArmadas = 0;        // Al principio no hay salas armadas

    // Mensaje de inicio del restaurante
    printf("Restaurante ABIERTO. Tiempo inicial: 0.000000 s\n");
    printf("Meseros disponibles: %d | Mesas a atender: %d\n\n", numMeseros, numMesas);

    // Imprime la cabecera de la tabla de trabajo
    printf("----------------------------------------------------------------------------\n");
    printf("|   NOMBRE   | IDENTIFICACIÓN |     MESAS     |   TIEMPOS   |  SALAS ARMADAS |\n");
    printf("----------------------------------------------------------------------------\n");

    // Se crean procesos hijos, uno por cada mesero
    for (int i = 0; i < numMeseros; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            struct timespec tIni, tFin;
            int inicio = i * (numMesas / numMeseros);        // Parte de las mesas que le toca a este mesero
            int fin = inicio + (numMesas / numMeseros);
            const char* nombre = nombresMeseros[i];
            int salasLocal = 0;                              // Contador de salas para este mesero
            int contadorMesas = 0;                           // Contador de mesas para formar una sala

            clock_gettime(CLOCK_MONOTONIC, &tIni);
            double tiempoRelIni = tiempoTranscurrido(tiempoInicioGlobal, tIni);

            // Se imprime la información del mesero cuando empiez
            printf("| %-10s | %14d | %5d - %-5d | inicio: %12.9fs |\n",
                   nombre, getpid(), inicio, fin - 1, tiempoRelIni);
            
            // Este ciclo simula que el mesero arma salas con sus mesas
            for (int j = inicio; j < fin; j++) {
                armarSala(&contadorMesas, &salasLocal);
            }
            // Suma las salas armadas por este proceso al total compartido
            __sync_fetch_and_add(&datos->salasArmadas, salasLocal);

            clock_gettime(CLOCK_MONOTONIC, &tFin);
            double tiempoRelFin = tiempoTranscurrido(tiempoInicioGlobal, tFin);

            // Imprime cuando termina y cuántas salas armó
            printf("| %-10s | %14d |_______________|  fin: %12.9fs    | %3d   |\n",
                   nombre, getpid(), tiempoRelFin, salasLocal);

            exit(0);            // Termina el proceso hijo
        }
    }
    
    // El proceso padre espera a que todos los meseros terminen
    for (int i = 0; i < numMeseros; i++) {
        wait(NULL);
    }

    // Se toma el tiempo final de la jornada
    clock_gettime(CLOCK_MONOTONIC, &tiempoFinGlobal);
    double duracionTotal = tiempoTranscurrido(tiempoInicioGlobal, tiempoFinGlobal);

    // Mensajes finales
    printf("----------------------------------------------------------------------------\n");
    printf("\nJornada finalizada.\n");
    printf("Salas armadas: %d\n", datos->salasArmadas);
    printf("Mesas atendidas: %d\n", numMesas);
    printf("Meseros que trabajaron: %d\n", numMeseros);
    printf("Restaurante CERRADO. Tiempo total: %.6f s\n", duracionTotal);
    // Libera la memoria compartida
    munmap(datos, sizeof(struct DatosCompartidos));
    return 0;
}
