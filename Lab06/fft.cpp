/**
 * @file   fft.cpp
 * @author Pratchaya Khansomboon (pratchaya.k.git@gmail.com)
 * @brief  FFT recursive and iterative implementation in C++
 * @date   2022-08-10
 *
 * @copyright Copyright (c) 2022
 */
#include <iostream>
#include <cstdint>
#include <cstddef>
#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include <algorithm>
#include <numeric>
#include <fstream>
#include <complex>
#include <array>
#include <string>
#include <sstream>

#include "fmt/format.h"

using f64 = double;
using fft_type = std::complex<f64>;
using fft_vec  = std::vector<fft_type>;

auto fft_r(fft_vec const& samples) -> fft_vec {
    using namespace std::complex_literals;
    auto const N = samples.size();
    if (N == 1) return samples;

    // Split by the middle
    auto const m = N / 2;

    // Generate 'Even' and 'Odd' list of complex numbers with
    // the size half of the sample size.
    fft_vec x_e(m, 0.0);  // Even samples
    fft_vec x_o(m, 0.0);  // Odd samples
    // Split the samples for the 'Even' and 'Odd' DFT
    for (std::size_t i = 0; i < m; i++) {
        x_e[i] = samples[2 * i];      // Even: x_{2m}
        x_o[i] = samples[2 * i + 1];  // Odd:  x_{2m + 1}
    }

    // Recursively split the rest of Evens and Odds
    fft_vec const f_e = fft_r(x_e);
    fft_vec const f_o = fft_r(x_o);

    // Compute the DFT
    fft_vec res(N, 0.0);
    for (std::size_t k = 0; k < m; k++) {
        auto const c = std::exp(-2.0i * M_PI * f64(k) / f64(N)) * f_o[k];

        res[k]     = f_e[k] + c;
        res[k + m] = f_e[k] - c;
    }

    return res;
}

auto fft_i([[maybe_unused]]fft_vec const& samples) -> fft_vec {
    return {};
}

auto complex_to_str_vec(fft_vec const& fft) -> std::vector<std::string> {
    using namespace std::string_literals;
    std::vector<std::string> str_list{};
    for (std::size_t i = 0; i < fft.size(); i++) {
        auto const f = fft[i];
        auto const pm_str = f.imag() < 0.0 ? "-"s : "+"s;
        str_list.emplace_back(fmt::format("{:.2f} {} {:.2f}", f.real(), pm_str, std::abs(f.imag())));
    }
    return str_list;
}

auto vec_to_json(std::vector<std::string> const& str_vec) -> std::string {
    std::string str{};
    str += "[\n";
    for (std::size_t i = 0; i < str_vec.size(); i++) {
        str += "  ";
        str += str_vec[i];
        if (i < str_vec.size() - 1) str += ",\n";
    }
    str += "\n]";
    return str;
}

auto main([[maybe_unused]]std::int32_t argc, [[maybe_unused]]char const* argv[]) -> std::int32_t {
    fft_vec const samples{1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0};
    auto fft = fft_r(samples);
    fmt::print("{}\n", vec_to_json(complex_to_str_vec(fft)));
    return 0;
}

