#!/usr/bin/env python3
import sys
import cmath
import math

def fft_r(samples: list[float]) -> list:
    n = len(samples);
    if n == 1:
        return samples

    m = int(n / 2)

    x_e = [0.0] * m
    x_o = [0.0] * m
    for i in range(m):
        x_e[i] = samples[2 * i]
        x_o[i] = samples[2 * i + 1]

    f_e = fft_r(x_e)
    f_o = fft_r(x_o)

    freqs = [0.0] * n
    for k in range(m):
        c = cmath.exp(-2.0j * cmath.pi * k / n) * f_o[k]

        freqs[k]     = f_e[k] + c
        freqs[k + m] = f_e[k] - c

    return freqs

def fft_i(samples: list[float]) -> list:
    def reverse_bit(b, radix = 2):
        n = 0
        for _ in range(radix + 1):
            n = n << 1
            n = n | (b & 1)
            b = b >> 1
        return n

    n       = len(samples)
    rev_i   = list(map(reverse_bit, [i for i in range(n)]))  # reverse bit order radix 2
    samples = list(map(lambda i : samples[i], rev_i))        # remap to new index order
    q       = round(math.log(n) / math.log(2))               # log_2(n)

    freqs = [0.0] * n
    for j in range(q):          # depth
        m = int(2.0 ** j)
        for k in range(2 ** (q - j - 1)):  # row
            s = int(k * 2 * m + 1)
            e = int((k + 1) * 2 * m)
            r = int(s + (e - s + 1) / 2)

            even = samples[s:r]
            odd  = samples[r-1:e + 1]

            it = 0
            for k in range(len()):
                freqs[s + k] =

    return []

def test_fft_rec():
    F_s = 8
    T_s = 1 / F_s
    N   = 8
    f   = 1

    s = range(N)
    s = list(map(lambda n: math.sin(2.0 * math.pi * f * T_s * n), s))
    print(s)
    print()
    a = fft_r(s)
    for v in a:
        print(round(abs(v)))

def test_fft_it():
    F_s = 8
    T_s = 1 / F_s
    N   = 8
    f   = 1

    s = range(N)
    s = list(map(lambda n: math.sin(2.0 * math.pi * f * T_s * n), s))
    a = fft_i(s)
    for v in a:
        print(round(abs(v)))

def main(args: list[str]):
    print('FFT')
    #test_fft_rec()
    test_fft_it()

if __name__ == '__main__':
    main(sys.argv)

