#include <stdio.h>
#include <pthread.h>

typedef struct {
    int start;
    int end;
} ThreadData;

int total_primes = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* worker(void *arg) {
    ThreadData *data = (ThreadData*)arg;
    int start = data->start;
    int end = data->end;
    int sum = 0;
    printf("Worker thread is processing numbers from %d to %d\n", start, end);

    for (int i = start; i <= end; i++) {
        sum += i;
    }
    printf("Total number of primes from %d to %d is: %d\n", start, end, sum);
    pthread_mutex_lock(&mutex);
    total_primes += sum;
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main() {
    ThreadData data1 = {1, 50}; 
    ThreadData data2 = {51, 100};

    pthread_t thread1, thread2;

    pthread_create(&thread1, NULL, worker, &data1);
    pthread_create(&thread2, NULL, worker, &data2);

    printf("Hello from main thread!\n");

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_mutex_destroy(&mutex);

    printf("Total primes from 1 to 100 is: %d\n", total_primes);
    printf("Program finished execution.\n");
    return 0;  
}
