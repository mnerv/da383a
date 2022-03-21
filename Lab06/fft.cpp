#include <iostream>
#include <cstdint>
#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include <algorithm>
#include <numeric>
#include <fstream>
#include <complex>

using fft_type = std::complex<double>;
using fft_vec  = std::vector<fft_type>;

auto fft_r(fft_vec const& samples) -> fft_vec {
    auto N = samples.size();
    if (N == 1) return samples;
    return {};
}

auto fft_i(fft_vec const& samples) -> fft_vec {
    return {};
}

auto main([[maybe_unused]]std::int32_t argc, [[maybe_unused]]char const* argv[]) -> std::int32_t {
    constexpr auto SAMPLE_COUNT = 8;
    std::vector<double> n{};
    std::generate_n(std::back_inserter(n), SAMPLE_COUNT, []{ static auto i = 0.0; return i++;});

    auto const f  = 1.0;       // frequency: Hz
    auto const fs = 8.0;       // sample frequency: Hz
    auto const Ts = 1.0 / fs;  // sample period: s

    std::vector<double> samples{};
    std::transform(std::begin(n), std::end(n), std::back_inserter(samples),
                   [&](auto const& n) {
        return std::sin(2.0 * M_PI * f * Ts * n) + std::sin(2.0 * M_PI * 2.0 * Ts * n) * 0.5;
    });

    // NOTE: Maybe only generate the half of the sample frequency
    std::vector<double> F{};  // frequency bin: 0 to fs
    std::generate_n(std::back_inserter(F), fs, [i = 0.0] () mutable {return i++;});

    // Discrete-Time Fourier Transform

    return 0;
}

