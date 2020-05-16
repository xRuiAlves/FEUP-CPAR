### Compilation Prerequisites:
- OpenMP lib
- OpenMPI lib
- GNU Make

### To compile:

For sequential and OpenMP: `make`
For OpenMPI: `mpic++ proj2_mpi.cpp -o proj2_mpi.out` or `mpic++ -O2 proj2_mpi.cpp -lm -lstdc++ -o proj2_mpi.out`

### To run:
* `./proj2.out <algorithm> <number> <execution_mode> [<block_size>]`
* `mpiexec ./proj2.out <number> <block_size>`
