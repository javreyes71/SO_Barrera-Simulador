#include "barrier.h"
#include <stdio.h>

void barrier_init(Barrier* barrier, int N) {
    barrier->N = N;
    barrier->count = 0;
    barrier->etapa = 0; 

    pthread_mutex_init(&barrier->mutex, NULL);
    pthread_cond_init(&barrier->cond, NULL);
}

void barrier_destroy(Barrier* barrier) {
    pthread_mutex_destroy(&barrier->mutex);
    pthread_cond_destroy(&barrier->cond);
}

void barrier_wait(Barrier* barrier) {
    // 1. Lock (Exclusión Mutua)
    pthread_mutex_lock(&barrier->mutex);

    // Capturar la etapa actual en una variable local 
    int mi_etapa = barrier->etapa; 

    barrier->count++;
    // Verificar si somos la última hebra
    if (barrier->count == barrier->N) {
        // --- CASO: SOY LA ÚLTIMA (Pista de diseño) ---
        
        // Incrementar etapa (esto invalidará la espera de las otras hebras)
        barrier->etapa++;
        // Resetear count
        barrier->count = 0;
        // Broadcast para despertar a todas
        pthread_cond_broadcast(&barrier->cond);
        
    } else {
        // --- CASO: NO SOY LA ÚLTIMA, TOCA ESPERAR ---
        
        // Usamos while para proteger contra "despertares espurios"
        while (mi_etapa == barrier->etapa) {
            pthread_cond_wait(&barrier->cond, &barrier->mutex);
        }
    }

    // 4. Unlock
    pthread_mutex_unlock(&barrier->mutex);
}
