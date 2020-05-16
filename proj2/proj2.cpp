#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <chrono>
#include <cmath>
#include <iostream>

typedef unsigned long u_long;

int sieve(u_long max_number, bool use_openmp = false);

int sieve_no_even(u_long max_number, bool use_openmp = false);

int sieve_no_even_block(const u_long lower_bound, const u_long upper_bound);

int sieve_blocks(u_long max_number, u_long block_size, bool use_openmp = false);

void printUsage(char* path) {
    printf(
        "usage: %s <algorithm> <max_number> <execution_type> [<block_size>]\n",
        path);
    printf("Algorithms:\n");
    printf("\t1: basic\n");
    printf("\t2: no even numbers\n");
    printf("\t3: block by block (requires block size input)\n");
    printf("Execution type:\n");
    printf("\t1: sequential\n");
    printf("\t2: OpenMP parallel\n");
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        printUsage(argv[0]);
        return 1;
    }

    int algorithm = atoi(argv[1]);
    u_long max_number = strtoul(argv[2], NULL, 10);
    int execution_type = atoi(argv[3]);

    if (algorithm < 1 || algorithm > 3) {
        printf("Invalid algorithm!\n");
        printUsage(argv[0]);
        return 2;
    }

    if (max_number <= 0) {
        printf("Max number must be positive!\n");
        printUsage(argv[0]);
        return 3;
    }

    int n_primes = -1;
    bool use_openmp = (execution_type == 2);

    switch (algorithm) {
        case 1:
            n_primes = sieve(max_number, use_openmp);
            break;
        case 2:
            n_primes = sieve_no_even(max_number, use_openmp);
            break;
        case 3: {
            if (argc < 4) {
                std::cerr << "No block size given!\n";
                printUsage(argv[0]);
                return 4;
            }
            int block_size = atoi(argv[4]);
            n_primes = sieve_blocks(max_number, block_size, use_openmp);
            break;
        }
    }

    printf("%d primes found!\n", n_primes);
    return 0;
}

int sieve(u_long max_number, bool use_openmp) {
    std::cout << "num procs: " << omp_get_num_procs() << std::endl;
    omp_set_num_threads(use_openmp ? omp_get_num_procs() : 1);

    auto start_time = std::chrono::high_resolution_clock::now();
    char* prime_list = new char[max_number + 1];
    memset(prime_list, 1, max_number + 1);

    const u_long max_number_sqrt = sqrt(max_number);
#pragma omp parallel for schedule(dynamic)
    for (u_long i = 2; i <= max_number_sqrt; ++i) {
        if (prime_list[i]) {
            for (u_long j = i * i; j <= max_number; j += i) {
                prime_list[j] = 0;
            }
        }
    }

    int primes_count = 0;
#pragma omp parallel for reduction(+ : primes_count)
    for (u_long i = 2; i <= max_number; ++i) {
        primes_count += prime_list[i];
    }

    auto end_time = std::chrono::high_resolution_clock::now();

    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                           end_time - start_time)
                           .count();

    std::cout << "Elapsed time: " << (duration_ms / 1000.0) << "s\n";
    // printf("Elapsed time: %fs\n", duration_s);

    delete[] prime_list;
    return primes_count;
}

int sieve_no_even(u_long max_number, bool use_openmp) {
    std::cout << "num procs: " << omp_get_num_procs() << std::endl;
    std::cout << "using openmp: " << use_openmp << std::endl;
    omp_set_num_threads(use_openmp ? omp_get_num_procs() : 1);

    auto start_time = std::chrono::high_resolution_clock::now();

    const u_long mem_size = (max_number - 1) / 2;

    char* prime_list = new char[mem_size + 1];
    memset(prime_list, 1, mem_size + 1);

    const u_long max_number_sqrt = sqrt(max_number);
#pragma omp parallel for schedule(dynamic)
    for (u_long i = 3; i <= max_number_sqrt; i += 2) {
        if (prime_list[i / 2]) {
            for (u_long j = i * i; j <= max_number; j += 2 * i) {
                prime_list[j / 2] = 0;
            }
        }
    }

    // Since we are skipping all evens, we must count 2 manually
    int primes_count = max_number >= 2 ? 1 : 0;
#pragma omp parallel for reduction(+ : primes_count)
    for (u_long i = 1; i <= mem_size; ++i) {
        primes_count += prime_list[i];
    }

    auto end_time = std::chrono::high_resolution_clock::now();

    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                           end_time - start_time)
                           .count();

    std::cout << "Elapsed time: " << (duration_ms / 1000.0) << "s\n";
    // printf("Elapsed time: %fs\n", duration_s);

    delete[] prime_list;
    return primes_count;
}

#define skip(i, n) \
    if (i % (n) == 0 && i >= (n) * (n)) continue;

int sieve_no_even_block(const u_long lower_bound, const u_long upper_bound) {
    const u_long mem_size = (upper_bound - lower_bound + 1) / 2;

    char* prime_list = new char[mem_size];
    memset(prime_list, 1, mem_size + 1);

    for (u_long i = 3; i * i <= upper_bound; i += 2) {
        // Skipping some known prime multiples
        skip(i, 3) skip(i, 5) skip(i, 7) skip(i, 11) skip(i, 13) skip(i, 17)
            skip(i, 19) skip(i, 23)

            // First multiple of i starting at the lower bound
            u_long first_mult = ((lower_bound + i - 1) / i) * i;
        if (first_mult < i * i) {
            first_mult = i * i;
        }

        // Ensuring that the value at which we start is odd
        if ((first_mult & 1) == 0) {
            first_mult += i;
        }

        for (u_long j = first_mult; j <= upper_bound; j += i * 2) {
            const u_long idx = j - lower_bound;
            prime_list[idx / 2] = 0;
        }
    }

    // number of primes in this block
    int primes_count = lower_bound <= 2 ? 1 : 0;
    for (u_long i = 0; i < mem_size; ++i) {
        primes_count += prime_list[i];
    }

    delete[] prime_list;
    return primes_count;
}

int sieve_blocks(u_long max_number, u_long block_size, bool use_openmp) {
    omp_set_num_threads(use_openmp ? omp_get_num_procs() : 1);

    int primes_total = 0;

    auto start_time = std::chrono::high_resolution_clock::now();

#pragma omp parallel for reduction(+ : primes_total)
    for (u_long lower_bound = 2; lower_bound <= max_number;
         lower_bound += block_size) {
        //
        const u_long upper_bound =
            std::min(lower_bound + block_size, max_number);

        primes_total += sieve_no_even_block(lower_bound, upper_bound);
    }

    auto end_time = std::chrono::high_resolution_clock::now();

    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                           end_time - start_time)
                           .count();

    std::cout << "Elapsed time: " << (duration_ms / 1000.0) << "s\n";

    return primes_total;
}
