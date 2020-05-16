#!/usr/bin/env bash

OUTPUT_FILE=data.csv

MIN_POWER_OF_2=25
MAX_POWER_OF_2=32

# "Compile pls" flag
if [[ "$1" = "-c" ]]; then
    make
    mpic++ proj2_mpi.cpp -o proj2_mpi.out
    shift
fi

# First arg or default to 1
ALGORITHM=${1:-1}

if [[ "$ALGORITHM" = "1" ]]; then
    ALGORITHM_NAME="basic"
elif [[ "$ALGORITHM" = "2" ]]; then
    ALGORITHM_NAME="noeven"
elif [[ "$ALGORITHM" = "3" ]]; then
    ALGORITHM_NAME="block"
else
    >&2 echo "Invalid algorithm"
    exit 1
fi

EXECUTION_MODE=${2:-1}

if [[ "$EXECUTION_MODE" = "1" ]]; then
    EXECUTION_MODE_NAME="sequential"
elif [[ "$EXECUTION_MODE" = "2" ]]; then
    EXECUTION_MODE_NAME="openmp"
elif [[ "$EXECUTION_MODE" = "3" ]]; then
    EXECUTION_MODE_NAME="mpi"
    if [[ "$ALGORITHM" != 3 ]]; then
        >&2 echo "Can only execute mpi with block algorithm!"
        exit 3
    fi
else
    >&2 echo "Invalid execution mode"
    exit 2
fi

BLOCK_SIZE=${3:-$((800*1024))}

echo "Algorithm: $ALGORITHM_NAME"
echo "Execution mode: $EXECUTION_MODE_NAME"
if [[ "$ALGORITHM" = "3" ]]; then echo "Block size: $BLOCK_SIZE"; fi
echo;

for ((i = MIN_POWER_OF_2; i <= MAX_POWER_OF_2; i++)); do
    current_power="$((2**i))"

    echo "## Executing for 2^$i=$current_power"

    if [[ "$EXECUTION_MODE" = 3 ]]; then
        mpiexec ./proj2_mpi.out "$current_power" "$BLOCK_SIZE" | sed -n -e "s/^Elapsed time: \(.*\)s.*/$ALGORITHM_NAME,$EXECUTION_MODE_NAME,$i,\1/p" | tee -a "$OUTPUT_FILE"
    else
        ./proj2.out "$ALGORITHM" "$current_power" "$EXECUTION_MODE" "$BLOCK_SIZE" | sed -n -e "s/^Elapsed time: \(.*\)s.*/$ALGORITHM_NAME,$EXECUTION_MODE_NAME,$i,\1/p" | tee -a "$OUTPUT_FILE"
    fi
done
