#!/usr/bin/env bash

for ((i = 1; i <= 3; i++)); do
    for ((j = 1; j <= 2; j++)); do
        # Both algorithm and execution mode range between 1 and 3
        # inside loop only goes to 2 since 3 is mpi and it only runs with blocks (3,3) so it's hardcoded outside
        ./run_experiment.sh "$i" "$j"
    done
done

./run_experiment.sh 3 3
