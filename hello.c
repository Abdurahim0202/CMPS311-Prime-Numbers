#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <pthread.h>

#define NUM_THREADS 4
#define N 100

// 1. Define a struct to pass arguments to each thread
typedef struct {
    int start;
    int end;
    int *primes;
} ThreadData;

// 2. Thread function (must return void* and take a single void* argument)
void* check_primes(void* arg) {
    ThreadData* data = (ThreadData*) arg;

    for (int i = data->start; i < data->end; i++) {
        bool is_prime = true;
        for (int j = 2; j <= sqrt(i); j++) {
            if (i % j == 0) {
                is_prime = false;
                break;
            }
        }
        if (is_prime && i > 1) {
            data->primes[i] = i;
        } else {
            data->primes[i] = 0;
        }
    }
    return NULL;
}

int main() {
    int primes[N];
    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];

    int chunk_size = N / NUM_THREADS;

    // 3. Create threads and divide the range among them
    for (int t = 0; t < NUM_THREADS; t++) {
        thread_data[t].start = t * chunk_size;
        // Last thread handles any remainder if N is not evenly divisible
        thread_data[t].end = (t == NUM_THREADS - 1) ? N : (t + 1) * chunk_size;
        thread_data[t].primes = primes;

        // Launch thread
        pthread_create(&threads[t], NULL, check_primes, &thread_data[t]);
    }

    // 4. Wait for all threads to finish execution
    for (int t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);
    }

    // 5. Print the prime numbers
    printf("Prime numbers up to %d:\n", N);
    for (int i = 0; i < N; i++) {
        if (primes[i] != 0) {
            printf("%d ", primes[i]);
        }
    }
    printf("\n");

    return 0;
}