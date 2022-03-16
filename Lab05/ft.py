#!/usr/bin/env python3
import sys
import cmath
import math

def fft_rec(samples: list[float]) -> list:
    n = len(samples);
    if n == 1:
        return samples

    m = int(n / 2)

    x_e = [0.0] * m
    x_o = [0.0] * m
    for i in range(m):
        x_e[i] = samples[2 * i]
        x_o[i] = samples[2 * i + 1]

    f_e = fft_rec(x_e)
    f_o = fft_rec(x_o)

    freqs = [0.0] * n
    for k in range(m):
        c = cmath.exp(-2.0j * cmath.pi * k / n) * f_o[k]

        freqs[k]     = f_e[k] + c
        freqs[k + m] = f_e[k] - c

    return freqs

def test_fft_rec():
    F_s = 8
    T_s = 1 / F_s
    N   = 8
    f   = 1

    s = range(N)
    s = list(map(lambda n: math.sin(2.0 * math.pi * f * T_s * n), s))
    print(s)
    print()
    a = fft_rec(s)
    for v in a:
        print(round(abs(v)))

def main():
    test_fft_rec()

if __name__ == '__main__':
    main()

