#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <stdint.h>

// Struct to pass bounds and memory addresses to each worker thread
typedef struct {
    int start;
    int end;
    int *primes;
    int local_count;
} ThreadData;

// Shared counter and mutex lock
int total_prime_count = 0;
pthread_mutex_t count_mutex;

// Fix #1: Corrected timing function syntax
double get_elapsed_seconds(struct timespec start, struct timespec end) {
    return ([suspicious link removed]_sec - [suspicious link removed]_sec) 
         + ([suspicious link removed]_nsec - [suspicious link removed]_nsec) / 1e9;
}

// Sequential prime checker for range [0, N)
int check_primes_sequential(int N, int *primes) {
    int count = 0;
    for (int i = 0; i < N; i++) {
        bool is_prime = true;
        for (int j = 2; j <= sqrt(i); j++) {
            if (i % j == 0) {
                is_prime = false;
                break;
            }
        }
        if (is_prime && i > 1) {
            primes[i] = i;
            count++;
        } else {
            primes[i] = 0;
        }
    }
    return count;
}

// Multithreaded worker function
void* check_primes_worker(void* arg) {
    ThreadData* data = (ThreadData*) arg;
    data->local_count = 0;

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
            data->local_count++;
        } else {
            data->primes[i] = 0;
        }
    }

    // Fix #3: Safe lock check — return early if lock fails without touching shared state
    int lock_ret = pthread_mutex_lock(&count_mutex);
    if (lock_ret != 0) {
        fprintf(stderr, "Error: pthread_mutex_lock failed with code %d\n", lock_ret);
        return NULL;
    }

    total_prime_count += data->local_count;

    int unlock_ret = pthread_mutex_unlock(&count_mutex);
    if (unlock_ret != 0) {
        fprintf(stderr, "Error: pthread_mutex_unlock failed with code %d\n", unlock_ret);
    }

    return NULL;
}

// Robust positive integer input parser using strtol
bool parse_positive_int(const char *str, int *out_val) {
    char *endptr;
    errno = 0;
    long val = strtol(str, &endptr, 10);

    if (errno != 0 || endptr == str || *endptr != '\0' || val <= 0 || val > 2147483647) {
        return false;
    }
    *out_val = (int)val;
    return true;
}

int main(int argc, char *argv[]) {
    int N = 100000;
    int num_threads = 4;

    // Fix #2: Correct argv[0] references for program usage
    if (argc == 3) {
        if (!parse_positive_int(argv[1], &N)) {
            fprintf(stderr, "Error: Invalid range parameter '%s'. Must be a positive integer.\n", argv[1]);
            return 1;
        }
        if (!parse_positive_int(argv[2], &num_threads)) {
            fprintf(stderr, "Error: Invalid thread count parameter '%s'. Must be a positive integer.\n", argv[2]);
            return 1;
        }
    } else if (argc != 1) {
        printf("Usage: %s <range_N> <num_threads>\n", argv[0]);
        return 1;
    } else {
        printf("Usage: %s <range_N> <num_threads>\n", argv[0]);
        printf("Using defaults: Exclusive Upper Bound N = %d, Threads = %d\n\n", N, num_threads);
    }

    // Clamp thread count if it exceeds range N
    if (num_threads > N) {
        printf("Notice: Requested %d threads for range N=%d. Clamping thread count to %d.\n", num_threads, N, N);
        num_threads = N;
    }

    // Memory size calculation and overflow check using size_t
    size_t primes_bytes = (size_t)N * sizeof(int);
    size_t threads_bytes = (size_t)num_threads * sizeof(pthread_t);
    size_t data_bytes = (size_t)num_threads * sizeof(ThreadData);

    if (N > 0 && primes_bytes / sizeof(int) != (size_t)N) {
        fprintf(stderr, "Error: Memory allocation size overflow for range N = %d.\n", N);
        return 1;
    }

    int *primes_seq = malloc(primes_bytes);
    int *primes_par = malloc(primes_bytes);
    pthread_t *threads = malloc(threads_bytes);
    ThreadData *thread_data = malloc(data_bytes);

    if (!primes_seq || !primes_par || !threads || !thread_data) {
        fprintf(stderr, "Error: Heap memory allocation failed.\n");
        free(primes_seq);
        free(primes_par);
        free(threads);
        free(thread_data);
        return 1;
    }

    int mutex_ret = pthread_mutex_init(&count_mutex, NULL);
    if (mutex_ret != 0) {
        fprintf(stderr, "Error: pthread_mutex_init failed with code %d\n", mutex_ret);
        free(primes_seq);
        free(primes_par);
        free(threads);
        free(thread_data);
        return 1;
    }

    // Sequential Baseline Timing
    struct timespec seq_start, seq_end;
    if (clock_gettime(CLOCK_MONOTONIC, &seq_start) != 0) {
        fprintf(stderr, "Error: clock_gettime failed for sequential start.\n");
    }

    int seq_count = check_primes_sequential(N, primes_seq);

    if (clock_gettime(CLOCK_MONOTONIC, &seq_end) != 0) {
        fprintf(stderr, "Error: clock_gettime failed for sequential end.\n");
    }
    double seq_time = get_elapsed_seconds(seq_start, seq_end);

    // Parallel Execution
    total_prime_count = 0;
    bool parallel_failed = false;
    struct timespec par_start, par_end;

    if (clock_gettime(CLOCK_MONOTONIC, &par_start) != 0) {
        fprintf(stderr, "Error: clock_gettime failed for parallel start.\n");
    }

    int chunk_size = N / num_threads;
    int created_threads = 0;

    for (int t = 0; t < num_threads; t++) {
        thread_data[t].start = t * chunk_size;
        thread_data[t].end = (t == num_threads - 1) ? N : (t + 1) * chunk_size;
        thread_data[t].primes = primes_par;

        int create_ret = pthread_create(&threads[t], NULL, check_primes_worker, &thread_data[t]);
        if (create_ret != 0) {
            fprintf(stderr, "Error: pthread_create failed for thread %d with code %d\n", t, create_ret);
            parallel_failed = true;
            break;
        }
        created_threads++;
    }

    // Join created threads and handle join failure
    for (int t = 0; t < created_threads; t++) {
        int join_ret = pthread_join(threads[t], NULL);
        if (join_ret != 0) {
            fprintf(stderr, "Error: pthread_join failed for thread %d with code %d\n", t, join_ret);
            parallel_failed = true;
        }
    }

    if (clock_gettime(CLOCK_MONOTONIC, &par_end) != 0) {
        fprintf(stderr, "Error: clock_gettime failed for parallel end.\n");
    }
    double par_time = get_elapsed_seconds(par_start, par_end);

    // Output and Verification
    printf("=== PRIMES SUMMARY (Exclusive Upper Bound N = %d) ===\n", N);

    if (parallel_failed) {
        printf("[FAIL] Parallel execution encountered synchronization or creation errors.\n");
    } else {
        if (N <= 100) {
            printf("Prime numbers in range [0, %d):\n", N);
            for (int i = 0; i < N; i++) {
                if (primes_par[i] != 0) {
                    printf("%d ", primes_par[i]);
                }
            }
            printf("\n\n");
        }

        printf("Sequential Execution Time : %.6f seconds (Primes found: %d)\n", seq_time, seq_count);
        printf("Parallel Execution Time   : %.6f seconds (Primes found: %d, Threads: %d)\n", par_time, total_prime_count, created_threads);
        
        if (par_time > 0) {
            printf("Speedup Ratio             : %.2fx\n", seq_time / par_time);
        }

        // Verification Check
        if (seq_count == total_prime_count) {
            printf("[VERIFICATION PASSED] Sequential and Parallel counts match (%d primes).\n", seq_count);
        } else {
            printf("[VERIFICATION FAILED] Count mismatch! Sequential=%d, Parallel=%d\n", seq_count, total_prime_count);
        }
    }

    // Resource Cleanup
    int destroy_ret = pthread_mutex_destroy(&count_mutex);
    if (destroy_ret != 0) {
        fprintf(stderr, "Warning: pthread_mutex_destroy failed with code %d\n", destroy_ret);
    }

    free(primes_seq);
    free(primes_par);
    free(threads);
    free(thread_data);

    return parallel_failed ? 1 : 0;
}