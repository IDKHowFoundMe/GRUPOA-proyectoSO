#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <time.h>

#define numMesas 30000
#define numMeseros 4
#define mesaSala 15

const char* nombresMeseros[] = {"Carlos", "Luis", "María", "Ana"};

struct SharedData {
    int salasArmadas;
};

double tiempoTranscurrido(struct timespec inicio, struct timespec fin) {
    return (fin.tv_sec - inicio.tv_sec) + (fin.tv_nsec - inicio.tv_nsec) / 1e9;
}

void armarSala(int* contadorMesas, int* contadorSalas) {
    (*contadorMesas)++;
    if (*contadorMesas == mesaSala) {
        (*contadorSalas)++;
        *contadorMesas = 0;
    }
}

int main() {
    struct timespec tiempoInicioGlobal, tiempoFinGlobal;
    clock_gettime(CLOCK_MONOTONIC, &tiempoInicioGlobal);

    struct SharedData* datos = mmap(NULL, sizeof(struct SharedData),
                                    PROT_READ | PROT_WRITE,
                                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    datos->salasArmadas = 0;

    printf("Restaurante ABIERTO. Tiempo inicial: 0.000000 s\n");
    printf("Meseros disponibles: %d | Mesas a atender: %d\n\n", numMeseros, numMesas);

    printf("----------------------------------------------------------------------------\n");
    printf("|   NOMBRE   | IDENTIFICACIÓN |     MESAS     |   TIEMPOS   |  SALAS ARMADAS |\n");
    printf("----------------------------------------------------------------------------\n");

    for (int i = 0; i < numMeseros; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            struct timespec tIni, tFin;
            int inicio = i * (numMesas / numMeseros);
            int fin = inicio + (numMesas / numMeseros);
            const char* nombre = nombresMeseros[i];
            int salasLocal = 0;
            int contadorMesas = 0;

            clock_gettime(CLOCK_MONOTONIC, &tIni);
            double tiempoRelIni = tiempoTranscurrido(tiempoInicioGlobal, tIni);

            printf("| %-10s | %14d | %5d - %-5d | inicio: %12.9fs |\n",
                   nombre, getpid(), inicio, fin - 1, tiempoRelIni);
            for (int j = inicio; j < fin; j++) {
                armarSala(&contadorMesas, &salasLocal);
            }

            __sync_fetch_and_add(&datos->salasArmadas, salasLocal);

            clock_gettime(CLOCK_MONOTONIC, &tFin);
            double tiempoRelFin = tiempoTranscurrido(tiempoInicioGlobal, tFin);

            printf("| %-10s | %14d |_______________|  fin: %12.9fs    | %3d   |\n",
                   nombre, getpid(), tiempoRelFin, salasLocal);

            exit(0);
        }
    }

    for (int i = 0; i < numMeseros; i++) {
        wait(NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &tiempoFinGlobal);
    double duracionTotal = tiempoTranscurrido(tiempoInicioGlobal, tiempoFinGlobal);

    printf("----------------------------------------------------------------------------\n");
    printf("\nJornada finalizada.\n");
    printf("Salas armadas: %d\n", datos->salasArmadas);
    printf("Mesas atendidas: %d\n", numMesas);
    printf("Meseros que trabajaron: %d\n", numMeseros);
    printf("Restaurante CERRADO. Tiempo total: %.6f s\n", duracionTotal);

    munmap(datos, sizeof(struct SharedData));
    return 0;
}
