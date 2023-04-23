// Samson DeVol, cs444 project 3
// basic producer/consumer problem solution with semaphores

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "eventbuf.h"

// initilizaed in main from command line args
int producer_count;
int consumer_count;
int producer_events_count;
int outstanding_count;

// initialized in main from sem_open_temp
sem_t *mutex;
sem_t *items;
sem_t *spaces;

// initialized in main from eventbuf_create
struct eventbuf *eb; 

sem_t *sem_open_temp(const char *name, int value){
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

void *run_producer(void *arg){
    int *id = arg;

    for (int i = 0; i < producer_events_count; i++){
        int event_id = *id * 100 + i;
        sem_wait(spaces);
        sem_wait(mutex);
        printf("P%d: adding event: %d\n", *id, event_id);
        eventbuf_add(eb, event_id);
        sem_post(mutex);
        sem_post(items);   
    }
    printf("P%d: exiting\n", *id);
    return NULL;
}

void *run_consumer(void *arg){
    int *id = arg;

    while(1){
        sem_wait(items);
        sem_wait(mutex);
        if(eventbuf_empty(eb)){
            sem_post(mutex);
            printf("C%d: exiting\n", *id);
            return NULL;
        }
        int event_num = eventbuf_get(eb);
        printf("C%d: got event: %d\n", *id, event_num);
        sem_post(mutex);
        sem_post(spaces);
    }
    return NULL;
}

int main(int argc, char* argv[]){
    
    if (argc != 5){
        printf("usage: ./pcseml num_of_producers num_of_consumers events_per_producer outstanding_at_a_time");
        exit(1);
    }

    // parse command line inputs
    producer_count = atoi(argv[1]);
    consumer_count = atoi(argv[2]);
    producer_events_count = atoi(argv[3]);
    outstanding_count = atoi(argv[4]);
    
    eb = eventbuf_create(); 
    mutex = sem_open_temp("mutex_sem", 1);
    items = sem_open_temp("items_sem", 0);
    spaces = sem_open_temp("spaces_sem", outstanding_count);

    pthread_t *producer_thread = calloc(producer_count, sizeof *producer_thread);
    int *producer_thread_id = calloc(producer_count, sizeof *producer_thread_id);
    pthread_t *consumer_thread = calloc(consumer_count, sizeof *consumer_thread);
    int *consumer_thread_id = calloc(consumer_count, sizeof *consumer_thread_id);

    // start correct number of producer threads
    for (int i = 0; i < producer_count; i++){
        producer_thread_id[i] = i;
        pthread_create(producer_thread + i, NULL, run_producer, producer_thread_id + i);
    }

    // start correct number of consumer threads
    for (int i = 0; i < consumer_count; i++){
        consumer_thread_id[i] = i;
        pthread_create(consumer_thread + i, NULL, run_consumer, consumer_thread_id + i);
    }

    // Wait for all producer threads to complete
    for (int i = 0; i < producer_count; i++)
        pthread_join(producer_thread[i], NULL);

    // Notify all the consumer threads that they're done
    for (int i = 0; i < consumer_count; i++)
        sem_post(items);

    // Wait for all consumer threads to complete
    for (int i = 0; i < consumer_count; i++)
        pthread_join(consumer_thread[i], NULL);

    // Free the event buffer
    eventbuf_free(eb);
    return 1;
}
