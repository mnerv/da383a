/**
 * @file   fft.cpp
 * @author Pratchaya Khansomboon (pratchaya.k.git@gmail.com)
 * @brief  FFT recursive and iterative implementation in C++20
 * @date   2022-08-10
 *
 * @copyright Copyright (c) 2022
 */
#include <cstdint>
#include <cstddef>
#include <cmath>

#include <vector>
#include <algorithm>
#include <numeric>
#include <fstream>
#include <complex>
#include <array>
#include <string>
#include <numbers>

#include "fmt/format.h"

using f64 = double;
using fft_type = std::complex<f64>;
using fft_vec  = std::vector<fft_type>;

namespace nrv {
    [[maybe_unused]]inline constexpr auto pi = std::numbers::pi;
}

/**
 * FFT recursive algorithm uses the classic divide and conquer algorithm. Which
 * makes it a straight forward to implement. With complexity O(N * log_2(N)).
 * However, the space complexity of this implementation is very good, the
 * function is called twice in every recursion and in total an array of length
 * 2^n has to be stored n times. This leads to space complexity of O(2^n * n).
 */
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
        auto const c = std::exp(-2.0i * nrv::pi * f64(k) / f64(N)) * f_o[k];

        res[k]     = f_e[k] + c;
        res[k + m] = f_e[k] - c;
    }

    return res;
}

/**
 * FFT iterative algorithm is not as straight forward as the recursive. The data
 * is divided into two arrays. The first one contains all even and the second
 * one all odd indices. If apply again to the subproblems, there are four arrays.
 * This is the FFT algorithm. For sample count of N = 8, the permutation is
 * log_2(N) = 3. The method is constructed for arbitary N = 2^n.
 *
 * The rearrange in the manner that'll solve the problem can be done with
 * 'bit inversion'/'reverse bit'. This means that the index k is written in
 * binary representation and then read backwards.
 */
auto fft_i(fft_vec const& samples) -> fft_vec {
    using namespace std::complex_literals;
    auto const N = samples.size();

    // Reverse the bits, N = 2^n
    // This is call radix-2 algorithm.
    auto const BIT_SIZE = std::size_t(std::log(f64(N)) / std::log(f64(2)));
    auto reverse_bit = [&](std::size_t b) {
        std::size_t n = 0;
        for (std::size_t i = 0; i < BIT_SIZE; i++) {
            n = n << 1;
            n = n | (b & 1);
            b = b >> 1;
        }
        return n;
    };

    // Split the original samples into even and odds part with reverse bit
    fft_vec fft_samples(N, 0.0);
    for (std::size_t i = 0; i < N; i++)
        fft_samples[i] = samples[reverse_bit(i)];

    // Compute the data permutation for the iterative FFT
    auto const q = BIT_SIZE;
    for (std::size_t j = 0; j < q; j++) {
        auto const m     = std::exp2(j);
        auto const k_lim = std::exp2(q - (j + 1));
        for (std::size_t k = 0; k < k_lim; k++) {
            auto const start = k * 2 * m;
            auto const end   = (k + 1) * 2 * m - 1;
            auto const mid   = start + (end - start + 1) / 2;

            for (std::size_t n = 0; n < m; n++) {
                auto const index = n + start;
                auto const z = std::exp(-1.0i * nrv::pi * f64(n) / f64(m)) * fft_samples[n + mid];
                auto const f = fft_samples[index];

                fft_samples[index]     = f + z;
                fft_samples[index + m] = f - z;
            }
        }
    }

    return fft_samples;
}

auto complex_to_str_vec(fft_vec const& fft) -> std::vector<std::string> {
    using namespace std::string_literals;
    std::vector<std::string> str_list{};
    for (std::size_t i = 0; i < fft.size(); i++) {
        auto const f = fft[i];
        auto const pm_str = f.imag() < 0.0 ? "-"s : "+"s;
        str_list.emplace_back(fmt::format("{:.2f} {} {:.2f}i", f.real(), pm_str, std::abs(f.imag())));
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

auto fft_to_csv_str() -> std::string {
    return "";
}

auto main([[maybe_unused]]std::int32_t argc, [[maybe_unused]]char const* argv[]) -> std::int32_t {
    fft_vec const samples{1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0};

    // FFT recursive
    //fmt::print("{}\n", vec_to_json(complex_to_str_vec(fft_r(samples))));

    // FFT iterative
    fmt::print("{}\n", vec_to_json(complex_to_str_vec(fft_i(samples))));

    return 0;
}

