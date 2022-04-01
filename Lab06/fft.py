#!/usr/bin/env python3
import sys
import cmath
import math
import time

def dft(bucket: list, samples: list):
    f = []
    N = len(samples)
    for k in bucket:
        sum = 0.0
        n = 0
        for a in samples:
            sum = sum + a * cmath.exp(-2.0j * cmath.pi * k * n / N)
            n += 1
        f.append(sum)
    return f

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

def reverse_bit(b: int, bit_size: int) -> int:
    n = 0
    for _ in range(bit_size):
        n = n << 1
        n = n | (b & 1)
        b = b >> 1
    return n

def fft_i(samples: list[float]) -> list:
    n        = len(samples)
    bit_size = int(math.log(n) / math.log(2))
    indexes  = [reverse_bit(i, bit_size) for i in range(n)]  # reverse bit order radix 2
    samples  = list(map(lambda i : samples[i], indexes))     # remap to new index order
    q        = bit_size

    freqs = samples  # FFT output

    for j in range(q):
        m = int(2 ** j)
        for k in range(2 ** (q - (j + 1))):
            start = k * 2 * m
            end   = (k + 1) * 2 * m - 1
            mid   = int(start + (end - start + 1) / 2)

            for n in range(m):
                index = n + start
                z = cmath.exp(-cmath.pi * 1.0j * n / m) * freqs[n + mid]
                f = freqs[index]
                freqs[index]     = f + z
                freqs[index + m] = f - z

    return freqs

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
        f = fft_r(samples)
    end = time.time()
    print(f'   1 000: {end - start} s')

    start = time.time()
    for i in range(10000):
        f = fft_r(samples)
    end = time.time()
    print(f'  10 000: {end - start} s')
    return

def test_fft_i(samples):
    print('FFT Iterative')

    start = time.time()
    for i in range(1000):
        f = fft_i(samples)
    end = time.time()
    print(f'   1 000: {end - start} s')

    start = time.time()
    for i in range(10000):
        f = fft_i(samples)
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
