#!/usr/bin/env python3
import sys

def reverse_bit(b: int, radix: int = 2) -> int:
    n = 0
    for i in range(radix + 1):
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

def main(argc: int, argv: list[str]):
    if argc < 2:
        print('error: not enough arguments')
        return 1

    DEFAULT_NUMBER = 0b0000_1111
    DEFAULT_RADIX  = 2

    number = parse_num(argv[1], DEFAULT_NUMBER)
    radix  = parse_num(argv[2], DEFAULT_RADIX) if argc > 2 else DEFAULT_RADIX

    rbit = reverse_bit(number, radix)

    in_fmt   = '{0:b}'.format(number).zfill(8)
    rbit_fmt = '{0:b}'.format(rbit).zfill(8)

    print(f'input:   0b{in_fmt}')
    print(f'reverse: 0b{rbit_fmt}')

if __name__ == '__main__':
    sys.exit(main(len(sys.argv), sys.argv))

