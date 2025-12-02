#ifndef BARRIER_H
#define BARRIER_H

#include <pthread.h>

typedef struct {
    pthread_mutex_t mutex;  
    pthread_cond_t cond;   
    
    int count;  
    int N;      
    int etapa;  
} Barrier;

void barrier_init(Barrier* barrier, int N);

void barrier_destroy(Barrier* barrier);

void barrier_wait(Barrier* barrier);

#endif // BARRIER_H

