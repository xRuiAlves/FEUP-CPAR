#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <chrono>
#include <cmath>
#include <iostream>

#include "mpi.h"

typedef unsigned long u_long;

void sieve_blocks(u_long max_number, u_long block_size);

void printUsage(char* path) {
    printf("usage: %s <max_number> <block_size>\n", path);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printUsage(argv[0]);
        return 1;
    }

    u_long max_number = strtoul(argv[1], NULL, 10);
    int block_size = atoi(argv[2]);

    if (max_number <= 0) {
        printf("Max number must be positive!\n");
        printUsage(argv[0]);
        return 3;
    }

    MPI_Init(&argc, &argv);
    sieve_blocks(max_number, block_size);

    MPI_Finalize();

    return 0;
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

void sieve_blocks(u_long max_number, u_long block_size) {
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int primes_global_total;
    int primes_local_total = 0;

    double start_time;
    if (rank == 0) {
        start_time = MPI_Wtime();
    }

    for (u_long lower_bound = 2 + (block_size * rank);
         lower_bound <= max_number; lower_bound += (block_size * size)) {
        //
        const u_long upper_bound =
            std::min(lower_bound + block_size, max_number);

        primes_local_total += sieve_no_even_block(lower_bound, upper_bound);
    }

    MPI_Reduce(&primes_local_total, &primes_global_total, 1, MPI_INT, MPI_SUM,
               0, MPI_COMM_WORLD);

    if (rank == 0) {
        double end_time = MPI_Wtime();
        std::cout << "Elapsed time: " << (end_time - start_time) << "s\n";
        printf("%d primes found!\n", primes_global_total);
    }
}
