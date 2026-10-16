
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include "prime_utils.h"


// Sequential prime search. Range comes from command-line arguments.
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <start> <end>\n", argv[0]);
        return 1;
    }

    int start = atoi(argv[1]);
    int end = atoi(argv[2]);

    // Start measuring time
    struct timespec startTime, endTime;

    clock_gettime(CLOCK_MONOTONIC, &startTime);

    // Scan the range sequentially and count all prime numbers.
    int count = 0;

    for (int i = start; i <= end; i++) {
        if (isPrime(i)) {
            count++;
        }
    }

    // End measuring time
    clock_gettime(CLOCK_MONOTONIC, &endTime);

    double elapsedTime =
        (endTime.tv_sec - startTime.tv_sec) +
        (endTime.tv_nsec - startTime.tv_nsec) / 1000000000.0;

    // Report the total prime count alongside the sequential timing.
    printf("Total primes: %d\n", count);
    printf("Execution time: %.6f seconds\n", elapsedTime);

    return 0;
}