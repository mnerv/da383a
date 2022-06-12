#!/usr/bin/env python3

# @file   dft.py
# @author Pratchaya Khansomboon (pratchaya.k.git@gmail.com)
# @brief  DFT Python implementation
# @date   2022-04-01
#
# @copyright Copyright (c) 2022
import sys
import cmath
import math

def dft(bucket: list, samples: list):
    frequencies = []
    N = len(samples)
    for k in bucket:
        sum = 0.0
        n = 0
        for a in samples:
            sum = sum + a * cmath.exp(-2.0j * cmath.pi * k * n / N)
            n += 1
        frequencies.append(sum)
    return frequencies

def main(argc: int, args: list[str]) -> int:
    print('DFT')

    samples = [1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0]

    n = [i for i in range(len(samples))]
    bucket = dft(n, samples)

    # Prints out the complex outputs
    for i in range(len(bucket)):
        f = bucket[i]
        real = '{:.2f}'.format(f.real)
        imag = '{:.2f}'.format(abs(f.imag))
        pm   = '+' if f.imag > 0 else '-'
        print(f'{real} {pm} {imag}i', end = '')
        if i < len(bucket) - 1:
            print('  ', end = '')
        else:
            print()

    # Prints out the magnitude and phase angle
    for i in range(len(bucket)):
        f = bucket[i]
        mag   = math.sqrt(f.real * f.real + f.imag * f.imag)
        phase = math.atan(f.imag / f.real) if f.real != 0 else 0

        fmag   = '{:.2f}'.format(mag)
        fphase = '{:.2f}'.format(phase)

        print(f'mag: {fmag}, phase: {fphase}', end = '\n')

    return 0

if __name__ == '__main__':
    sys.exit(main(len(sys.argv), sys.argv))

