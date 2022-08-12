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

using fft_type = std::complex<double>;
using fft_vec  = std::vector<fft_type>;

auto fft_r(fft_vec const& samples) -> fft_vec {
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
        auto const c = fft_type{
            std::cos(-2.0 * M_PI * k / N),
            std::sin(-2.0 * M_PI * k / N)
        } * f_o[k];

        res[k]     = f_e[k] + c;
        res[k + m] = f_e[k] - c;
    }

    return res;
}

auto fft_i([[maybe_unused]]fft_vec const& samples) -> fft_vec {
    return {};
}

auto main([[maybe_unused]]std::int32_t argc, [[maybe_unused]]char const* argv[]) -> std::int32_t {
    fft_vec const samples{1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0};

    auto res = fft_r(samples);
    for (std::size_t i = 0; i < res.size(); i++) {
        std::cout << res[i].real() << " + " << res[i].imag() << "i";
        if (i < res.size() - 1) std::cout << ", ";
        else std::cout << "\n";
    };
    return 0;
}

