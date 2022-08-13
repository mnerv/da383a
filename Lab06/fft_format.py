# @file   fft_format.py
# @author Pratchaya Khansomboon (pratchaya.k.git@gmail.com)
# @brief  String format
# @date   2022-08-13
#
# @copyright Copyright (c) 2022
import cmath
import math

def complex_to_str_vec(fft: list[complex]) -> list[str]:
    list_str: list[str] = []
    N = len(fft)
    for i in range(N):
        f = fft[i]
        real = '{:.2f}'.format(f.real)
        imag = '{:.2f}'.format(abs(f.imag))
        pm   = '-' if f.imag < 0.0 else '+'
        list_str.append(f'{real} {pm} {imag}i')
    return list_str

def vec_to_json(str_list: list[str]) -> str:
    output = "[\n"
    size = len(str_list)
    for i in range(size):
        output += "  ";
        output += str_list[i]
        if (i < size - 1):
            output += ",\n";
    output += "\n]"
    return output

def fft_csv(fft: list[complex]) -> str:
    def compute(fft: list[complex]) -> tuple[list[float], list[float]]:
        mags:   list[float] = []
        phases: list[float] = []

        for i in range(len(fft)):
            f = fft[i]
            mag   = math.sqrt(f.real * f.real + f.imag * f.imag)
            phase = math.atan(f.imag / f.real) if f.real != 0 else 0

            mags.append(mag)
            phases.append(phase)  # radian
        return (mags, phases)

    def num_to_str(value: float) -> str:
        return '{:.2f}'.format(value)

    vec_complex_str    = complex_to_str_vec(fft)
    vec_mag, vec_phase = compute(fft)
    vec_mag_str   = list(map(num_to_str, vec_mag))
    vec_phase_str = list(map(num_to_str, vec_phase))
    assert(len(vec_complex_str) == len(vec_mag_str) == len(vec_phase_str))

    # add headers
    vec_complex_str.insert(0, "complex")
    vec_mag_str.insert(0, "magnitude")
    vec_phase_str.insert(0, "phase (radian)")

    # format csv
    line_count = len(vec_phase_str)

    # calculate max column width per column value
    max_col_widths: list[int] = [0] * 3

    output = ""
    for i in range(line_count):
        output += vec_complex_str[i] + ', '
        output += vec_mag_str[i]     + ', '
        output += vec_phase_str[i]   + '\n'

    return output

