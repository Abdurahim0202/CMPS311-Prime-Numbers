CC = gcc
CFLAGS = -Wall -Wextra -O2
LDLIBS = -lm

all: primes parallel_primes

primes: primes.c prime_utils.c prime_utils.h
	$(CC) $(CFLAGS) primes.c prime_utils.c -o primes $(LDLIBS)

parallel_primes: parallel_primes.c prime_utils.c prime_utils.h
	$(CC) $(CFLAGS) parallel_primes.c prime_utils.c -o parallel_primes -pthread $(LDLIBS)

clean:
	rm -f primes parallel_primes

.PHONY: all clean