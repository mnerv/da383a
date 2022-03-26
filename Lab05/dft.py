#!/usr/bin/env python3
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

def main(args: list[str]) -> int:
    SAMPLE_COUNT = 8
    n = [i for i in range(SAMPLE_COUNT)]

    f  = 1.0
    fs = 8.0       # sampling frequency [Hz]
    Ts = 1.0 / fs  # sampling period    [s]

    # generate sinusoidal signal
    def signal(n):
        return math.sin(2.0 * math.pi * f * Ts * n)
    samples = list(map(signal, n))  # x[n]

    bucket = n
    bucket = dft(bucket, samples)

    bucket = list(map(lambda x : round(abs(x)), bucket))

    print(bucket)

    return 0

if __name__ == '__main__':
    sys.exit(main(sys.argv))

