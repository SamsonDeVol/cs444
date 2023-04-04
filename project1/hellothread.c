// Samson DeVol, cs444 project 1
// basic multithreading program using POSIX library

#include <stdio.h>
#include <assert.h>
#include <pthread.h>

void *run(void *arg){
    char* thread_name = arg;
    printf("%s: 0\n", thread_name);
    printf("%s: 1\n", thread_name);
    printf("%s: 2\n", thread_name);
    printf("%s: 3\n", thread_name);
    printf("%s: 4\n", thread_name);
    return NULL;
}

int main(void) {

    // main thread begin
    printf("Launching threads\n");

    // initialize threads
    pthread_t t1, t2;
    int status;
    // create the threads with appropriate thread names
    status = pthread_create(&t1, NULL, run, "thread 1"); assert(status == 0);
    status = pthread_create(&t2, NULL, run, "thread 2");

    // wait for each thread to complete
    status = pthread_join(t1, NULL); assert(status == 0);
    status = pthread_join(t2, NULL); assert(status == 0);

    // all threads complete, close main thread
    printf("Threads complete!\n");

    return 0;
}