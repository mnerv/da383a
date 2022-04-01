#!/usr/bin/env python3

# @file   fft.py
# @author Pratchaya Khansomboon (pratchaya.k.git@gmail.com)
# @brief  Test recursive, iterative FFT and DFT algorithm
# @date   2022-04-01
#
# @copyright Copyright (c) 2022
import sys
import cmath
import math
import time

from dft import dft
from fftr import fftr
from ffti import ffti

def test_dft(samples):
    bucket = [n for n in range(len(samples))]

    print('DFT')

    start = time.time()
    for i in range(1000):
        f = dft(bucket, samples)
    end = time.time()
    print(f'   1 000: {end - start} s')

    start = time.time()
    for i in range(10000):
        f = dft(bucket, samples)
    end = time.time()
    print(f'  10 000: {end - start} s')

    return

def test_fft_r(samples):
    print('FFT Recursive')

    start = time.time()
    for i in range(1000):
        f = fftr(samples)
    end = time.time()
    print(f'   1 000: {end - start} s')

    start = time.time()
    for i in range(10000):
        f = fftr(samples)
    end = time.time()
    print(f'  10 000: {end - start} s')
    return

def test_fft_i(samples):
    print('FFT Iterative')

    start = time.time()
    for i in range(1000):
        f = ffti(samples)
    end = time.time()
    print(f'   1 000: {end - start} s')

    start = time.time()
    for i in range(10000):
        f = ffti(samples)
    end = time.time()
    print(f'  10 000: {end - start} s')

    return

def main(argc: int, args: list[str]) -> int:
    F_s = 8
    T_s = 1 / F_s
    N   = 8
    f   = 1
    samples = [math.sin(2.0 * math.pi * f * T_s * n) for n in range(N)]

    test_dft(samples)
    test_fft_r(samples)
    test_fft_i(samples)

    return 0

if __name__ == '__main__':
    sys.exit(main(len(sys.argv), sys.argv))

