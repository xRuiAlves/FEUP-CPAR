### Compilation Prerequisites:
- OpenMP lib
- OpenMPI lib
- GNU Make

### To compile:

For sequential and OpenMP: `make`
For OpenMPI (WIP): `mpic++ x.cpp` or `mpic++ -O2 x.cpp -lm -lstdc++`

### To run:
* ./proj2.out <algorithm> <number> <execution_mode>
* mpiexec -np 8 ./proj2.out <number> <debug_flag> (to confirm)
