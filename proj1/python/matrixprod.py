#!/usr/bin/env python3

from pypapi import papi_high
from pypapi import events as papi_events

from time import time
import sys


def papiStartCounters():
    papi_high.start_counters([
        papi_events.PAPI_L1_DCM,
        papi_events.PAPI_L2_DCM,
    ])


def papiStopCounters():
    return papi_high.stop_counters()


def multiplyMatrix(mat_size, is_line=False):
    mat1 = [1.0] * (mat_size * mat_size)
    mat2 = [1.0 + i for i in range(mat_size) for _ in range(mat_size)]
    mat_res = [0.0] * (mat_size * mat_size)

    start_time = time()
    if is_line:
        for i in range(mat_size):
            for k in range(mat_size):
                for j in range(mat_size):
                    mat_res[i*mat_size + j] += mat1[i*mat_size + k] * mat2[k*mat_size + j]
    else:
        for i in range(mat_size):
            for j in range(mat_size):
                acc = 0
                for k in range(mat_size):
                    acc += mat1[i*mat_size + k] * mat2[k*mat_size + j]
                mat_res[i*mat_size + j] = acc

    end_time = time()
    return '{:3.3f}'.format(end_time - start_time)

def printUsage(path):
    print(f'usage: {path} <algorithm> <matrix_size>')
    print('algorithms:')
    print('\t1: regular')
    print('\t2: line by line')

if __name__ == "__main__":
    if len(sys.argv) < 3:
        printUsage(sys.argv[0])
        sys.exit(1)

    algorithm = int(sys.argv[1])
    mat_size = int(sys.argv[2])

    if algorithm < 1 or algorithm > 3:
        print("Invalid algorithm!")
        printUsage(sys.argv[0])
        exit(2)

    papiStartCounters()

    if algorithm == 1:
        time = multiplyMatrix(mat_size)
    elif algorithm == 2:
        time = multiplyMatrix(mat_size, True)      

    [l1_dcm, l2_dcm] = papiStopCounters()

    print(f'{time} {l1_dcm} {l2_dcm}')
