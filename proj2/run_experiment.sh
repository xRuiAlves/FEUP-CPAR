#!/usr/bin/env bash

OUTPUT_FILE=data.csv

MIN_POWER_OF_2=25
MAX_POWER_OF_2=32

# First arg or default to 1
ALGORITHM=${1:-1}

for ((i = MIN_POWER_OF_2; i <= MAX_POWER_OF_2; i++)); do
    current_power="$((2**i))"

    echo "## Executing for 2^$i=$current_power"

    ./proj2.out "$ALGORITHM" "$current_power" | sed -n -e "s/^Elapsed time: \(.*\)s.*/sequential,$ALGORITHM,$i,\1/p" | tee -a "$OUTPUT_FILE"
done
