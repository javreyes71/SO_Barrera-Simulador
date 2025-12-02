#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include "barrier.h"

Barrier barrera;
int num_etapas;

void* thread_routine(void* arg) {
    long tid = (long)arg;

    for (int e = 0; e < num_etapas; e++) {
        int tiempo = rand() % 200000; 
        usleep(tiempo);
        printf("[%ld] esperando en etapa %d\n", tid, e);
        barrier_wait(&barrera);
        printf("[%ld] paso barrera en etapa %d\n", tid, e);
    }

    return NULL;
}

int main(int argc, char* argv[]) {
    // 1. Configuración de parámetros (Defectos: N=5, E=4)
    int N = 5;
    int E = 4;

    // Si se pasan argumentos, se sobrescriben los defectos
    if (argc > 1) {
        N = atoi(argv[1]);
    }
    if (argc > 2) {
        E = atoi(argv[2]);
    }
    
    // Guardamos E en la variable global para las hebras
    num_etapas = E;

    // semilla de aleatoriedad
    srand(time(NULL));

    printf("=== Verificación de Barrera ===\n");
    printf("Hebras (N): %d\n", N);
    printf("Etapas (E): %d\n", E);
    printf("-------------------------------\n");

    pthread_t hebras[N];

    // Inicializar la barrera
    barrier_init(&barrera, N);

    // Crear hebras
    for (long i = 0; i < N; i++) {
        if (pthread_create(&hebras[i], NULL, thread_routine, (void*)i) != 0) {
            perror("Error creando hebra");
            exit(1);
        }
    }

    // Esperar 
    for (int i = 0; i < N; i++) {
        pthread_join(hebras[i], NULL);
    }

    // Destruir barrera
    barrier_destroy(&barrera);

    printf("-------------------------------\n");
    printf("Fin de la ejecución.\n");

    return 0;
}