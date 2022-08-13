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

import fft_format as fft_fmt

def fftr(samples: list[complex]) -> list[complex]:
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
    samples = [1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0]
    FFT = fftr(samples)

    # Prints out the complex outputs
    # print(vec_to_json(complex_to_str_vec(FFT)))

    # Prints out the magnitude and phase angle
    print(fft_fmt.fft_csv(FFT))

    return 0

if __name__ == '__main__':
    sys.exit(main(len(sys.argv), sys.argv))
