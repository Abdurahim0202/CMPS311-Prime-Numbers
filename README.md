# Parallel Prime Number Search

**Course:** CMPS 311 – Operating Systems  
**Project:** Group Project I – Fall 2026  
**Track:** 3B – Parallel Prime Number Search  
**Team Members:** Abdurahim Sanginov, Jorge Munoz

## Project Overview

This project implements a parallel prime-number search using C and POSIX threads (pthreads) on Ubuntu Linux. The main goal is to compare the performance of sequential and multithreaded execution.

The program takes a range of integers and the number of threads as command-line arguments. It first counts the prime numbers sequentially, then divides the same range among multiple threads and compares their execution times.

Both implementations use the same prime-checking function to ensure a fair comparison.

## Project Files

- `parallel_primes.c` — Main program containing thread creation, range partitioning, synchronization, input validation, and performance comparison.
- `primes.c` — Standalone sequential prime-counting program.
- `prime_utils.c` — Shared functions for checking prime numbers and counting primes sequentially.
- `prime_utils.h` — Function declarations for the shared utilities.
- `Makefile` — Automates compilation and cleanup.

## Requirements

- Ubuntu Linux (or WSL with Ubuntu)
- GCC compiler
- GNU Make
- POSIX threads library

## Compilation

Open a terminal in the project directory and run:

```bash
make
```

This compiles both the sequential and parallel programs.

To remove the compiled executables:

```bash
make clean
```

## Running the Program

The main program uses the following command format:

```bash
./parallel_primes <start> <end> <threads>
```

Where:
- `start` — Beginning of the range (inclusive).
- `end` — End of the range (inclusive).
- `threads` — Number of threads to create (1–64).

Example:

```bash
./parallel_primes 1 10000000 4
```

The program displays the total number of primes found by both implementations, their execution times, and the calculated speedup.

To run the standalone sequential version:

```bash
./primes 1 10000000
```

## Implementation

### Sequential Execution

The sequential implementation checks each number in the given range using the `isPrime()` function and counts how many are prime.

### Parallel Execution

The program divides the numeric range into approximately equal sections and assigns each section to a worker thread using `pthread_create()`.

Each thread counts the primes in its assigned section independently. The local counts are then added to a shared variable protected by a mutex (`pthread_mutex_lock()` and `pthread_mutex_unlock()`) to prevent race conditions.

The main thread uses `pthread_join()` to wait for all workers to finish.

### Performance Measurement

Execution times are measured using `clock_gettime()` with `CLOCK_MONOTONIC`.

The program calculates speedup using:

**Speedup = Sequential Execution Time / Parallel Execution Time**

Running the program with different thread counts allows us to observe how parallelism affects performance.

## Input Validation and Error Handling

The program checks for invalid command-line arguments, including noninteger inputs, invalid ranges, nonpositive thread counts, and thread counts exceeding 64 or the size of the numeric range.

It also checks for errors during thread creation and avoids reporting incomplete results if a thread cannot be created.

## Expected Output

For the range from 1 to 10,000,000, both implementations should find **664,579 prime numbers**.

Execution times vary depending on the computer and system workload.

## Development Environment

The project was developed and tested using C, GCC, and Ubuntu Linux through Windows Subsystem for Linux (WSL).