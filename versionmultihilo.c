#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define numMesas 30000
#define numMeseros 4
#define mesaSala 15

// Nombres de los meseros
const char* nombresMeseros[] = {"Carlos", "Ana", "Luis", "María"};

// Mutex para proteger el acceso a la variable global de salas armadas
pthread_mutex_t mutexSalas;
int salasArmadasGlobal = 0;    // Contador global de salas armadas

// Estructura para pasar argumentos a los hilos
struct ThreadArgs {
    int idMesero;    // ID del mesero
    int inicioMesa;    // Indice de la primera mesa que atendera
    int finMesa;    // Indice de la última mesa que atendera
    struct timespec tiempoInicioGlobal;    // Tiempo de inicio global
};

//Función para calcular el tiempo transcurrido entre dos instantes
double tiempoTranscurrido(struct timespec inicio, struct timespec fin) {
    return (fin.tv_sec - inicio.tv_sec) + (fin.tv_nsec - inicio.tv_nsec) / 1e9;
}

//Función para armar una sala, incrementando contadores
void armarSala(int* contadorMesas, int* contadorSalas) {
    (*contadorMesas)++;
    if (*contadorMesas == mesaSala) {
        (*contadorSalas)++;
        *contadorMesas = 0;
    }
}

// Función que ejecuta cada hilo para atender mesas
void* atenderMesas(void* argVoid) {
    struct ThreadArgs* args = (struct ThreadArgs*)argVoid;    // Cast de argumentos
    struct timespec tIni, tFin;    // Variables para medir tiempo
    const char* nombre = nombresMeseros[args->idMesero];    //Nombre del mesero
    unsigned long idHilo = (unsigned long)pthread_self();    // ID del hilo actual

    int contadorMesas = 0;    // Contador local de mesas atendidas
    int salasLocal = 0;    // Contador local de salas armadas

    clock_gettime(CLOCK_MONOTONIC, &tIni);    // Tiempo de inicio para este hilo

    // Bucle para atender las mesas asignadas
    for (int i = args->inicioMesa; i < args->finMesa; i++) {
        armarSala(&contadorMesas, &salasLocal);    // Armar sala
    }

    clock_gettime(CLOCK_MONOTONIC, &tFin);    //Tiempo de fin para este hilo

    // Calcular el tiempo relativo de inicio y fin
    double tiempoRelIni = tiempoTranscurrido(args->tiempoInicioGlobal, tIni);
    double tiempoRelFin = tiempoTranscurrido(args->tiempoInicioGlobal, tFin);

    // Bloquear el mutex antes de modificar la variable global
    pthread_mutex_lock(&mutexSalas);
    salasArmadasGlobal += salasLocal;    // Actualizar el contador global de salas
    pthread_mutex_unlock(&mutexSalas);    // Desbloquear el mutex

    // Imprimir el resultado del mesero
    printf("| %-10s | %14lu | %5d - %-5d | ini: %8.6fs | fin: %8.6fs |   %4d        |\n",
           nombre, idHilo, args->inicioMesa, args->finMesa - 1,
           tiempoRelIni, tiempoRelFin, salasLocal);

    pthread_exit(NULL);    //Terminar el hilo
}

int main() {
    pthread_t hilos[numMeseros];    // Array para almacenar los hilos
    struct ThreadArgs args[numMeseros];    //Array para los argumentos de los hilos
    struct timespec tiempoInicioGlobal, tiempoFinGlobal;    //Variables para medir tiempo global

    pthread_mutex_init(&mutexSalas, NULL);    // Inicializar el mutex
    clock_gettime(CLOCK_MONOTONIC, &tiempoInicioGlobal);    // Obtener tiempo de inicio global

    // Mensaje de apertura del restaurante
    printf(" Restaurante ABIERTO. Tiempo inicial: 0.000000 s\n");
    printf(" Meseros disponibles: %d | Mesas a atender: %d\n\n", numMeseros, numMesas);

    // Cabecera de la tabla de resultados
    printf("------------------------------------------------------------------------------------------------------\n");
    printf("|   NOMBRE   | IDENTIFICACIÓN |     MESAS     |    INICIO    |     FIN      | SALAS ARMADAS |\n");
    printf("------------------------------------------------------------------------------------------------------\n");

    // Crear hilos para cada mesero
    for (int i = 0; i < numMeseros; i++) {
        args[i].idMesero = i;    // Asignar ID al mesero
        args[i].inicioMesa = i * (numMesas / numMeseros);    // Calcular inicio de mesas
        args[i].finMesa = args[i].inicioMesa + (numMesas / numMeseros);    // Calcular fin de mesas
        args[i].tiempoInicioGlobal = tiempoInicioGlobal;    // Pasar tiempo de inicio global

        // Crear el hilo para atender mesas
        if (pthread_create(&hilos[i], NULL, atenderMesas, (void*)&args[i]) != 0) {
            perror("Error creando hilo");    // Manejo de errores
            exit(EXIT_FAILURE);
        }
    }

    // Esperar a que todos los hilos terminen
    for (int i = 0; i < numMeseros; i++) {
        pthread_join(hilos[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &tiempoFinGlobal);    // Obtener tiempo de fin global
    double duracionTotal = tiempoTranscurrido(tiempoInicioGlobal, tiempoFinGlobal);    //Calcular duración total

    // Mensaje de cierre del restaurante
    printf("------------------------------------------------------------------------------------------------------\n");
    printf("\n Jornada finalizada.\n");
    printf(" Salas armadas: %d\n", salasArmadasGlobal);
    printf(" Mesas atendidas: %d\n", numMesas);
    printf(" Meseros que trabajaron: %d\n", numMeseros);
    printf(" Restaurante CERRADO. Tiempo total: %.6f s\n", duracionTotal);

    pthread_mutex_destroy(&mutexSalas);    // Destruir el mutex
    return 0;    // Fin del programa
}
