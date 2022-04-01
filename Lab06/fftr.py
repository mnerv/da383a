#!/usr/bin/env python3

# @file   fftr.py
# @author Pratchaya Khansomboon (pratchaya.k.git@gmail.com)
# @brief  Recursive FFT implementation
# @date   2022-04-01
#
# @copyright Copyright (c) 2022
import sys
import cmath
import math

def fftr(samples: list[float]) -> list:
    n = len(samples);
    if n == 1:
        return samples

    m = int(n / 2)

    x_e = [0.0] * m
    x_o = [0.0] * m
    for i in range(m):
        x_e[i] = samples[2 * i]
        x_o[i] = samples[2 * i + 1]

    f_e = fftr(x_e)
    f_o = fftr(x_o)

    freqs = [0.0] * n
    for k in range(m):
        c = cmath.exp(-2.0j * cmath.pi * k / n) * f_o[k]

        freqs[k]     = f_e[k] + c
        freqs[k + m] = f_e[k] - c

    return freqs

def main(argc: int, args: list[str]) -> int:
    print('FFT Recursive')

    samples = [1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0]
    FFT = fftr(samples)

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
