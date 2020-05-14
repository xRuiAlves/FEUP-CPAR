#include <stdio.h>
#include <stdlib.h>

#include "algorithms.h"

void printUsage(char* path) {
    printf(
        "usage: %s <algorithm> <max_number> <execution_type> [<block_size>]\n",
        path);
    printf("algorithms:\n");
    printf("\t1: basic\n");
    printf("\t2: no even numbers\n");
    printf("\t3: block by block (requires block size input)\n");
    printf("execution type: WIP\n");
    printf("\t1: sequential\n");
    printf("\t2: OpenMP parallel\n");
    printf("\t3: OpenMPI distributed\n");
}

/*
 * Time measurements:
 * - Sequential: clock()
 * - OpenMPI: WPI_wtime
 * - OpenMP: ?
 */

int main(int argc, char* argv[]) {
    if (argc < 4) {
        printUsage(argv[0]);
        return 1;
    }

    int algorithm = atoi(argv[1]);
    unsigned long max_number = strtoul(argv[2], NULL, 10);
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
    bool use_openmp = false;

    if (execution_type == 2) {
        use_openmp = true;
    }

    switch (algorithm) {
        case 1:
            n_primes = sieve(max_number, use_openmp);
            break;
        case 2:
            n_primes = sieve_no_even(max_number, use_openmp);
            break;
        case 3: {
            printf("Block is WIP\n");
            return 4;
            // if (argc < 4) {
            //     printUsage(argv[0]);
            //     return 4;
            // }
            // printf("Block is WIP\n");
            // int block_size = atoi(argv[3]);
            // break;
        }
    }

    printf("%d primes found!\n", n_primes);
}
