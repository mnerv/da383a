#!/usr/bin/env python3

# @file   ffti.py
# @author Pratchaya Khansomboon (pratchaya.k.git@gmail.com)
# @brief  Iterative FFT implementation
# @date   2022-04-01
#
# @copyright Copyright (c) 2022
import sys
import cmath
import math

def reverse_bit(b: int, bit_size: int) -> int:
    n = 0
    for _ in range(bit_size):
        n = n << 1
        n = n | (b & 1)
        b = b >> 1
    return n

def ffti(samples: list[float]) -> list:
    n        = len(samples)
    bit_size = int(math.log(n) / math.log(2))
    samples  = [samples[reverse_bit(i, bit_size)] for i in range(n)]
    q        = bit_size

    for j in range(q):
        m = int(2 ** j)
        for k in range(2 ** (q - (j + 1))):
            start = k * 2 * m
            end   = (k + 1) * 2 * m - 1
            mid   = int(start + (end - start + 1) / 2)

            for n in range(m):
                index = n + start
                z = cmath.exp(-cmath.pi * 1.0j * n / m) * samples[n + mid]
                f = samples[index]
                samples[index]     = f + z
                samples[index + m] = f - z

    return samples

def main(argc: int, args: list[str]) -> int:
    print('FFT Iterative')

    samples = [1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0]
    FFT = ffti(samples)

    # Prints out the complex outputs
    for i in range(len(FFT)):
        f = FFT[i]
        real = '{:.2f}'.format(f.real)
        imag = '{:.2f}'.format(abs(f.imag))
        pm   = '+' if f.imag > 0 else '-'
        print(f'{real} {pm} {imag}i', end = '')
        if i < len(FFT) - 1:
            print('  ', end = '')
        else:
            print()

    # Prints out the magnitude and phase angle
    for i in range(len(FFT)):
        f = FFT[i]
        mag   = math.sqrt(f.real * f.real + f.imag * f.imag)
        phase = math.atan(f.imag / f.real) if f.real != 0 else 0

        fmag   = '{:.2f}'.format(mag)
        fphase = '{:.2f}'.format(phase)

        print(f'mag: {fmag}, phase: {fphase}', end = '\n')

    return 0

if __name__ == '__main__':
    sys.exit(main(len(sys.argv), sys.argv))
