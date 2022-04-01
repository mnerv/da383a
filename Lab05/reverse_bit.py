#!/usr/bin/env python3
import sys
import math

def reverse_bit(b: int, bit_size: int) -> int:
    n = 0
    for _ in range(bit_size):
        n = n << 1
        n = n | (b & 1)
        b = b >> 1
    return n

def is_base_2(text: str) -> bool:
    return text.find('0b') != -1

def parse_num(text: str, default: int):
    try:
        if is_base_2(text):
            return int(text.replace('0b', ''), 2)
        else:
            return int(text)
    except ValueError:
        return default

def is_power_of_2(n: int):
    return (n != 0) and (n & (n - 1)) == 0

def main(argc: int, argv: list[str]):
    if argc < 2:
        print('error: not enough arguments')
        return 1

    size = parse_num(argv[1], 8)

    if not is_power_of_2(size):
        print('error: size needs to be power of 2!')
        return 1

    bit_size = int(math.log(size) / math.log(2))
    n  = [x for x in range(size)]
    rn = list(map(lambda x : reverse_bit(x, bit_size), n))

    for i in range(size):
        print(f'{n[i]}    {rn[i]}')

if __name__ == '__main__':
    sys.exit(main(len(sys.argv), sys.argv))

