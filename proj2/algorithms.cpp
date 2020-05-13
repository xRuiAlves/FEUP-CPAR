#include "algorithms.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int sieve(int max_number) {
    clock_t start_time = clock();
    char* primeList = new char[max_number + 1];
    memset(primeList, 1, max_number + 1);

    for (int i = 2; i * i <= max_number; ++i) {
        if (primeList[i]) {
            for (int j = i * i; j <= max_number; j += i) {
                primeList[j] = 0;
            }
        }
    }

    int primes_count = 0;
    for (int i = 2; i <= max_number; ++i) {
        primes_count += primeList[i];
    }

    clock_t end_time = clock();

    double time_s = ((double)(end_time - start_time) / CLOCKS_PER_SEC);
    printf("Elapsed time: %fs\n", time_s);

    delete[] primeList;
    return primes_count;
}

int sieve_no_even(int max_number) {
    clock_t start_time = clock();

    int mem_size = (max_number - 1) / 2;

    char* primeList = new char[mem_size + 1];
    memset(primeList, 1, mem_size + 1);

    for (int i = 3; i * i <= max_number; i += 2) {
        if (primeList[i / 2]) {
            for (int j = i * i; j <= max_number; j += 2 * i) {
                primeList[j / 2] = 0;
            }
        }
    }

    // Since we are skipping all evens, we must count 2 manually
    int primes_count = max_number >= 2 ? 1 : 0;
    for (int i = 1; i <= mem_size; ++i) {
        primes_count += primeList[i];
    }

    clock_t end_time = clock();

    double time_s = ((double)(end_time - start_time) / CLOCKS_PER_SEC);
    printf("Elapsed time: %fs\n", time_s);

    delete[] primeList;
    return primes_count;
}
