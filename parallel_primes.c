#include <stdio.h>
#include <pthread.h>
#include "prime_utils.h"
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <errno.h>

typedef struct {
    long long start;
    long long end;
} ThreadData;

long long total_primes = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* worker(void *arg) {
    ThreadData *data = (ThreadData*)arg;
    long long start = data->start;
    long long end = data->end;
    long long count = 0;

    for (long long i = start; i <= end; i++) {
        if (isPrime((int)i)) {
            count++;
        }
    }
    printf("Total number of primes from %lld to %lld is: %lld\n", start, end, count);
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
    if (numThreads > 64) {
        printf("Error: Maximum number of threads is 64.\n");
        return 1;
    }
    if (numThreads > totalNumbers) {
        printf("Error: Number of threads cannot exceed range size.\n");
        return 1;
    }

    long long chunkSize = totalNumbers / numThreads;
    long long remainder = totalNumbers % numThreads;
    long long currentStart = start;

    ThreadData data[numThreads];
    pthread_t threads[numThreads];

    // Start measuring time
    clock_gettime(CLOCK_MONOTONIC, &startTime);

    int createdThreads = 0;
    int failedThreads = 0;
    for (int i = 0; i < numThreads; i++) {
        data[i].start = currentStart;
        data[i].end = currentStart + chunkSize - 1;
        if (remainder > 0) {
            data[i].end++;
            remainder--;
        }
        currentStart = data[i].end + 1;
        int status = pthread_create(&threads[i], NULL, worker, &data[i]);

        if (status != 0) {
            fprintf(stderr, "Failed to create thread %d (error %d)\n", i, status);
            failedThreads++;
            break;
        } else {
            createdThreads++;
        }
    }

    for (int i = 0; i < createdThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    if (failedThreads > 0) {
        fprintf(stderr, "Failed to create %d threads. Exiting.\n", failedThreads);
        pthread_mutex_destroy(&mutex);
        return 1;
    }
    

    // End measuring time
    clock_gettime(CLOCK_MONOTONIC, &endTime);
    pthread_mutex_destroy(&mutex);
    
    double elapsed =
    (endTime.tv_sec - startTime.tv_sec) +
    (endTime.tv_nsec - startTime.tv_nsec) / 1e9;

    printf("Total primes from %d to %d is: %lld\n", start, end, total_primes);
    printf("Elapsed time: %.6f seconds\n", elapsed);


    return 0;
}
