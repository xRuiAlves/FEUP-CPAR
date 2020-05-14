#!/usr/bin/env bash

for ((i = 1; i <= 3; i++)); do
    for ((j = 1; j <= 3; j++)); do
        # Both algorithm and execution mode range between 1 and 3
        ./run_experiment.sh "$i" "$j"
    done
done
