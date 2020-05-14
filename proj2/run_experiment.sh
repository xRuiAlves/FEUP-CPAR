#!/usr/bin/env bash

OUTPUT_FILE=data.csv

MIN_POWER_OF_2=25
MAX_POWER_OF_2=32

# First arg or default to 1
ALGORITHM=${1:-1}

if [[ "$ALGORITHM" = "1" ]]; then
    ALGORITHM_NAME="basic"
elif [[ "$ALGORITHM" = "2" ]]; then
    ALGORITHM_NAME="noeven"
elif [[ "$ALGORITHM" = "3" ]]; then
    ALGORITHM_NAME="block"
else
    echo "Invalid algorithm"
    exit 1
fi

EXECUTION_MODE=${2:-1}

if [[ "$EXECUTION_MODE" = "1" ]]; then
    EXECUTION_MODE_NAME="sequential"
elif [[ "$EXECUTION_MODE" = "2" ]]; then
    EXECUTION_MODE_NAME="openmp"
elif [[ "$EXECUTION_MODE" = "3" ]]; then
    EXECUTION_MODE_NAME="mpi"
else
    echo "Invalid execution mode"
    exit 2
fi

echo "Algorithm: $ALGORITHM_NAME"
echo "Execution mode: $EXECUTION_MODE_NAME"
echo;

for ((i = MIN_POWER_OF_2; i <= MAX_POWER_OF_2; i++)); do
    current_power="$((2**i))"

    echo "## Executing for 2^$i=$current_power"

    ./proj2.out "$ALGORITHM" "$current_power" "$EXECUTION_MODE" | sed -n -e "s/^Elapsed time: \(.*\)s.*/$ALGORITHM_NAME,$EXECUTION_MODE_NAME,$i,\1/p" | tee -a "$OUTPUT_FILE"
done
