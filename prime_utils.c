#include "prime_utils.h"
#include <math.h>

// Trial-division prime test used by both sequential and parallel searches.
bool isPrime(int num){
    if(num < 2){
        return false;
    }

    for (int i = 2; i <= sqrt(num); i++) {
        if (num % i == 0) {
            return false;
        }
    }
    return true; 
}

// Sequential scan of the range; used to compare against the multithreaded result.
long long countPrimes(int start, int end) {
    long long count = 0;

    for (long long i = start; i <= end; i++) {
        if (isPrime((int)i)) {
            count++;
        }
    }

    return count;
}