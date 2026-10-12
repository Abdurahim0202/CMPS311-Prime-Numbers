
#include <stdio.h>
#include <math.h>

int main() {
    int n = 100; 
    int primes[n];
    for (int i = 0; i < n; i++) {
        bool is_prime = true; 
        for (int j = 2; j < sqrt(i)+1; j++) {
            if (i % j == 0) {
                is_prime = false;
                break;
            }
        }
        if (is_prime && i > 1) {
            primes[i] = i; 
        } else {
            primes[i] = 0; 
        }
    }
    for (int i = 0; i < n; i++) {
        if (primes[i] != 0) {
            printf("%d ", primes[i]);
        }
    }
    return 0;
}
