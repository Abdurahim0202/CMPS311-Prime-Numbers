#include <stdio.h>
#include <pthread.h>
#include "prime_utils.h"
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <errno.h>

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
    int count = 0;

    for (int i = start; i <= end; i++) {
        if (isPrime(i)) {
            count++;
        }
    }
    printf("Total number of primes from %d to %d is: %d\n", start, end, count);
    pthread_mutex_lock(&mutex);
    total_primes += count;
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int parseInteger(const char *text, int *result) {
    char *endptr;

    errno = 0;
    long value = strtol(text, &endptr, 10);

    if (endptr == text || *endptr != '\0' ||
        errno == ERANGE || value < INT_MIN || value > INT_MAX) {
        return 0;
    }

    *result = (int)value;
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <start> <end> <threads>\n", argv[0]);
        return 1;
    }
    
    int start, end, numThreads;
    if (!parseInteger(argv[1], &start) || 
        !parseInteger(argv[2], &end) || 
        !parseInteger(argv[3], &numThreads)) {
        printf("Error: Invalid integer input.\n");
        return 1;
    }
    
    // Start measuring time
    struct timespec startTime, endTime;
    long long totalNumbers = (long long)end - start + 1;

    if(start > end || numThreads <= 0) {
        printf("Invalid range or number of threads.\n");
        return 1;
    }
    if (numThreads > totalNumbers) {
        printf("Error: Number of threads cannot exceed range size.\n");
        return 1;
    }

    int chunkSize = totalNumbers / numThreads;
    int remainder = totalNumbers % numThreads;
    int currentStart = start;

    ThreadData data[numThreads];
    pthread_t threads[numThreads];

    // Start measuring time
    clock_gettime(CLOCK_MONOTONIC, &startTime);

    for (int i = 0; i < numThreads; i++) {
        data[i].start = currentStart;
        data[i].end = currentStart + chunkSize - 1;
        if (remainder > 0) {
            data[i].end++;
            remainder--;
        }
        currentStart = data[i].end + 1;
        pthread_create(&threads[i], NULL, worker, &data[i]);
    }

    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
    }
    

    // End measuring time
    clock_gettime(CLOCK_MONOTONIC, &endTime);
    pthread_mutex_destroy(&mutex);
    
    double elapsed =
    (endTime.tv_sec - startTime.tv_sec) +
    (endTime.tv_nsec - startTime.tv_nsec) / 1e9;

    printf("Total primes from %d to %d is: %d\n", start, end, total_primes);
    printf("Elapsed time: %.6f seconds\n", elapsed);


    return 0;
}
