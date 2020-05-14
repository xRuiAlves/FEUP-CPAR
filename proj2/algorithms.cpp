#include "algorithms.h"

#include <omp.h>
#include <stdlib.h>
#include <string.h>

#include <chrono>
#include <cmath>
#include <iostream>

int sieve(unsigned long max_number, bool use_openmp) {
    std::cout << "num procs: " << omp_get_num_procs() << std::endl;
    std::cout << "using openmp: " << use_openmp << std::endl;
    omp_set_num_threads(use_openmp ? omp_get_num_procs() : 1);

    auto start_time = std::chrono::high_resolution_clock::now();
    char* primeList = new char[max_number + 1];
    memset(primeList, 1, max_number + 1);

    const unsigned long max_number_sqrt = sqrt(max_number);
#pragma omp parallel for schedule(dynamic)
    for (unsigned long i = 2; i <= max_number_sqrt; ++i) {
        if (primeList[i]) {
            for (unsigned long j = i * i; j <= max_number; j += i) {
                primeList[j] = 0;
            }
        }
    }

    int primes_count = 0;
#pragma omp parallel for reduction(+ : primes_count)
    for (unsigned long i = 2; i <= max_number; ++i) {
        primes_count += primeList[i];
    }

    auto end_time = std::chrono::high_resolution_clock::now();

    auto duration_s = std::chrono::duration_cast<std::chrono::milliseconds>(
                          end_time - start_time)
                          .count();

    std::cout << "Elapsed time: " << (duration_s / 1000.0) << "s\n";
    // printf("Elapsed time: %fs\n", duration_s);

    delete[] primeList;
    return primes_count;
}

int sieve_no_even(unsigned long max_number, bool use_openmp) {
    std::cout << "num procs: " << omp_get_num_procs() << std::endl;
    std::cout << "using openmp: " << use_openmp << std::endl;
    omp_set_num_threads(use_openmp ? omp_get_num_procs() : 1);

    auto start_time = std::chrono::high_resolution_clock::now();

    unsigned long mem_size = (max_number - 1) / 2;

    char* primeList = new char[mem_size + 1];
    memset(primeList, 1, mem_size + 1);

    const unsigned long max_number_sqrt = sqrt(max_number);
#pragma omp parallel for schedule(dynamic)
    for (unsigned long i = 3; i <= max_number_sqrt; i += 2) {
        if (primeList[i / 2]) {
            for (unsigned long j = i * i; j <= max_number; j += 2 * i) {
                primeList[j / 2] = 0;
            }
        }
    }

    // Since we are skipping all evens, we must count 2 manually
    int primes_count = max_number >= 2 ? 1 : 0;
#pragma omp parallel for reduction(+ : primes_count)
    for (unsigned long i = 1; i <= mem_size; ++i) {
        primes_count += primeList[i];
    }

    auto end_time = std::chrono::high_resolution_clock::now();

    auto duration_s = std::chrono::duration_cast<std::chrono::milliseconds>(
                          end_time - start_time)
                          .count();

    std::cout << "Elapsed time: " << (duration_s / 1000.0) << "s\n";
    // printf("Elapsed time: %fs\n", duration_s);

    delete[] primeList;
    return primes_count;
}
