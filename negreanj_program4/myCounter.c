#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

// PROGRAM DESCRIPTION
// the producer and consumer do the exact same thing
// they just both alternate in incrementation
// switch_off variable starts as -1 such that producer starts by default
// conditionals are there as a sasfeguards if a thread is taking too long to modify switch_off
// because of this, <THREAD>: waiting on <CONDITION VAR> may print, but it may not other times
// all external variables (myCount and switch_off) are modified when the mutex is locked

// max size for myCount
#define SIZE 10

// initialization of myCounmt
int myCount = 0;

// initialization of switch_off variable, which describes which thread is currently running
// -1 means producer is running, 1 means consumer is running, 0 means both should terminate
int switch_off = -1;

// initialization of myMutex
pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;

// initialization of myCond1, waited for by producer, and myCond2, waited for by consumer
pthread_cond_t myCond1;
pthread_cond_t myCond2;

// consumer thread function
void *consumer_thread_func (void* args) {
    // infinite while loop
    while (1) {
        
        // terminate if switch_off is 0
        if (switch_off == 0) {
            break;
        }

        // lock the mutex
        pthread_mutex_lock(&myMutex);
        printf("CONSUMER: myMutex locked\n");

        // check if producer is technically supposed to be running
        if (switch_off == -1) {
            // wait for producer to finish
            printf("CONSUMER: waiting on myCond2\n");
            pthread_cond_wait(&myCond2, &myMutex);
        }

        // if myCount reaches max size
        if (myCount == SIZE) {
            // signal for producer to continue execution
            printf("CONSUMER: signaling myCond1\n");
            pthread_cond_signal(&myCond1);

            // set switch_off to termination
            switch_off = 0;

            // unlock the mutex
            printf("CONSUMER: myMutex unlocked\n");
            pthread_mutex_unlock(&myMutex);

            // skip the rest and restart loop
            continue;
        }

        // keep track of old myCount value
        int temp = myCount;

        // increment myCount and print change
        myCount ++;
        printf("myCount: %d -> %d\n", (myCount-1), myCount);

        // toggle switch_off so that producer can now run
        switch_off *= -1;

        // signal for producer to continue execution
        printf("CONSUMER: signaling myCond1\n");
        pthread_cond_signal(&myCond1);
        
        // unlock the mutex
        // note how all thread functionality is within the bounds of the lock and unlock
        printf("CONSUMER: myMutex unlocked\n");
        pthread_mutex_unlock(&myMutex);
    }
}

int main () {

    // indicate program start
    printf("PROGRAM START\n");

    // create and start consumer thread running consumer thread function
    pthread_t consumer_thread;
    pthread_create(&consumer_thread, NULL, consumer_thread_func, NULL);
    printf("CONSUMER THREAD CREATED\n");
    
    // infitite while loop
    while (1) {

        // terminate if switch_off is 0
        if (switch_off == 0) {
            break;
        }

        // lock the mutex
        pthread_mutex_lock(&myMutex);
        printf("PRODUCER: myMutex locked\n");

        // check if consumer is technically supposed to be running
        if (switch_off == 1) {
            // wait for consumer to finish
            printf("CONSUMER: waiting on myCond1\n");
            pthread_cond_wait(&myCond1, &myMutex);
        }

        // if myCount reaches max size
        if (myCount == SIZE) {
            // signal for consumer to continue execution
            printf("PRODUCER: signaling myCond2\n");
            pthread_cond_signal(&myCond2);

            // set switch_off to termination
            switch_off = 0;

            // unlock the mutex
            printf("PRODUCER: myMutex unlocked\n");
            pthread_mutex_unlock(&myMutex);

            // skip the rest and restart loop
            continue;
        }

        // keep track of old myCount value
        int temp = myCount;

        // increment myCount and print change
        myCount ++;
        printf("myCount: %d -> %d\n", temp, myCount);

        // toggle switch_off so that consumer can now run
        switch_off *= -1;

        // signal for consumer to continue execution
        printf("PRODUCER: signaling myCond2\n");
        pthread_cond_signal(&myCond2);
        
        // unlock the mutex
        // note how all thread functionality is within the bounds of the lock and unlock
        printf("PRODUCER: myMutex unlocked\n");
        pthread_mutex_unlock(&myMutex);
    }

    // join consumer thread after it finishes full execution
    pthread_join(consumer_thread, NULL);

    // destroy myMutex
    pthread_mutex_destroy(&myMutex);

    // indicate program end
    printf("PROGRAM END\n");

    // return with no errors
    return 0;
}