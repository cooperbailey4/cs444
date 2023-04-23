#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include "eventbuf.c"
#include "eventbuf.h"

struct eventbuf *eventbuf;

sem_t *mutex;
sem_t *items;
sem_t *spaces;

int Enum;

sem_t *sem_open_temp(const char *name, int value)
{
    sem_t *sem;

    // Create the semaphore
    if ((sem = sem_open(name, O_CREAT, 0600, value)) == SEM_FAILED)
        return SEM_FAILED;

    // Unlink it so it will go away after this process exits
    if (sem_unlink(name) == -1) {
        sem_close(sem);
        return SEM_FAILED;
    }

    return sem;
}

void *produce(void *arg) {
    for (int i = 0; i < Enum; i++) {
        int event = *(int *)arg * 100 + i;
        sem_wait(spaces);
        sem_wait(mutex);
        printf("P%d: adding event %d\n", *(int *)arg, event);
        eventbuf_add(eventbuf, event);
        sem_post(mutex);
        sem_post(items);
    }
    printf("P%d: exiting\n", *(int *)arg);
    pthread_exit(NULL);
}

void *consume(void *arg) {
    while (1) {
        sem_wait(items);
        sem_wait(mutex);
        if(eventbuf_empty(eventbuf)){
            break;
        }
        int event = eventbuf_get(eventbuf);
        printf("C%d: got event %d\n", *(int *)arg, event);
        sem_post(mutex);
        sem_post(spaces);
    }
    printf("C%d: exiting\n", *(int *)arg);
    pthread_exit(NULL);
}

int main(int arg, char **argc) {
    if (!(arg == 5)){
        exit(1);
    }

    int Pnum = atoi(argc[1]);
    int Cnum = atoi(argc[2]);
    Enum = atoi(argc[3]);
    int OEnm = atoi(argc[4]);

    eventbuf = eventbuf_create();

    mutex = sem_open_temp("mutex", 1);
    items = sem_open_temp("items", 0);
    spaces = sem_open_temp("spaces", OEnm);

    //Producer and consumer thread memory allocations
    pthread_t *producers = calloc(Pnum, sizeof(pthread_t));
    pthread_t *consumers = calloc(Cnum, sizeof(pthread_t));

    int *Pthread_id = calloc(Pnum, sizeof *Pthread_id);
    int *Cthread_id = calloc(Cnum, sizeof *Cthread_id);

    // Producer thread creation
    for (int i = 0; i < Pnum; i++) {
        Pthread_id[i] = i;
        pthread_create(producers + i, NULL, produce, Pthread_id + i);
    }


    // Consumer thread creation
    for (int i = 0;i < Cnum; i++) {
        Cthread_id[i] = i;
        pthread_create(consumers + i, NULL, consume, Cthread_id + i);
    }

    // Wait for all producers to complete
    for (int i = 0; i < Pnum; i++){
        pthread_join(producers[i], NULL);
    }
    for (int i = 0; i < Cnum; i++){
        sem_post(mutex);
        sem_post(items);
    }


    // Wait for all consumers to complete
    for (int i = 0; i < Cnum; i++) {
        pthread_join(consumers[i], NULL);
    }

    eventbuf_empty(eventbuf);
    sem_close(mutex);
    sem_close(items);
    sem_close(spaces);

}


// For example, to launch with 2 producers and 4 consumers, and have each producer make 5 events, with a maximum outstanding event count of 2:
// ./pcseml 2 4 5 2
