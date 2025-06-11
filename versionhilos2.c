#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define numMesas 30000
#define numMeseros 4
#define mesaSala 15

const char* nombresMeseros[] = {"Carlos", "Ana", "Luis", "María"};

pthread_mutex_t mutexSalas;
int salasArmadasGlobal = 0;

struct ThreadArgs {
    int idMesero;
    int inicioMesa;
    int finMesa;
    struct timespec tiempoInicioGlobal;
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

void* atenderMesas(void* argVoid) {
    struct ThreadArgs* args = (struct ThreadArgs*)argVoid;
    struct timespec tIni, tFin;
    const char* nombre = nombresMeseros[args->idMesero];
    unsigned long idHilo = (unsigned long)pthread_self();

    int contadorMesas = 0;
    int salasLocal = 0;

    clock_gettime(CLOCK_MONOTONIC, &tIni);

    for (int i = args->inicioMesa; i < args->finMesa; i++) {
        armarSala(&contadorMesas, &salasLocal);
    }

    clock_gettime(CLOCK_MONOTONIC, &tFin);

    double tiempoRelIni = tiempoTranscurrido(args->tiempoInicioGlobal, tIni);
    double tiempoRelFin = tiempoTranscurrido(args->tiempoInicioGlobal, tFin);

    pthread_mutex_lock(&mutexSalas);
    salasArmadasGlobal += salasLocal;
    pthread_mutex_unlock(&mutexSalas);

    printf("| %-10s | %14lu | %5d - %-5d | ini: %8.6fs | fin: %8.6fs |   %4d        |\n",
           nombre, idHilo, args->inicioMesa, args->finMesa - 1,
           tiempoRelIni, tiempoRelFin, salasLocal);

    pthread_exit(NULL);
}

int main() {
    pthread_t hilos[numMeseros];
    struct ThreadArgs args[numMeseros];
    struct timespec tiempoInicioGlobal, tiempoFinGlobal;

    pthread_mutex_init(&mutexSalas, NULL);
    clock_gettime(CLOCK_MONOTONIC, &tiempoInicioGlobal);

    printf(" Restaurante ABIERTO. Tiempo inicial: 0.000000 s\n");
    printf(" Meseros disponibles: %d | Mesas a atender: %d\n\n", numMeseros, numMesas);

    printf("------------------------------------------------------------------------------------------------------\n");
    printf("|   NOMBRE   | IDENTIFICACIÓN |     MESAS     |    INICIO    |     FIN      | SALAS ARMADAS |\n");
    printf("------------------------------------------------------------------------------------------------------\n");

    for (int i = 0; i < numMeseros; i++) {
        args[i].idMesero = i;
        args[i].inicioMesa = i * (numMesas / numMeseros);
        args[i].finMesa = args[i].inicioMesa + (numMesas / numMeseros);
        args[i].tiempoInicioGlobal = tiempoInicioGlobal;

        if (pthread_create(&hilos[i], NULL, atenderMesas, (void*)&args[i]) != 0) {
            perror("Error creando hilo");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < numMeseros; i++) {
        pthread_join(hilos[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &tiempoFinGlobal);
    double duracionTotal = tiempoTranscurrido(tiempoInicioGlobal, tiempoFinGlobal);

    printf("------------------------------------------------------------------------------------------------------\n");
    printf("\n Jornada finalizada.\n");
    printf(" Salas armadas: %d\n", salasArmadasGlobal);
    printf(" Mesas atendidas: %d\n", numMesas);
    printf(" Meseros que trabajaron: %d\n", numMeseros);
    printf(" Restaurante CERRADO. Tiempo total: %.6f s\n", duracionTotal);

    pthread_mutex_destroy(&mutexSalas);
    return 0;
}
