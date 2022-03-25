/**
 * @file   filter.hpp
 * @author Pratchaya Khansomboon (pratchaya.k.git@gmail.com)
 * @brief  Filter coefficient frequency response test
 * @date   2022-03-25
 *
 * @copyright Copyright (c) 2022
 */
#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include <algorithm>
#include <numeric>
#include <fstream>
#include <string>

#include "types.hpp"
#include "filter_coeffs.hpp"
#include "queue.hpp"

namespace envi {
auto clear = "\033[H\033[2J";
auto hide  = "\033[?25l";
auto show  = "\033[?25h";
auto reset = "\u001b[0m";

auto ecg(nerv::f64 x) -> nerv::f64 {
    auto f = 1.5;
    return std::sin(2.0 * M_PI * f * 4.0 * x) *
           std::pow(0.5 * (std::sin(2.0 * M_PI * f * x) + 1.0), 5.0);
}
}

auto main([[maybe_unused]]nerv::i32 argc, [[maybe_unused]]char const* argv[]) -> nerv::i32 {
    constexpr auto fs = 1'000.;
    constexpr auto Ts = 1.0 / fs;
    constexpr std::size_t sample_count = 1000;

    std::vector<nerv::f64> n{};
    std::generate_n(std::back_inserter(n), sample_count, [i = 0.0]() mutable { return i++;});

    std::vector<nerv::f64> samples{};
    std::vector<nerv::f64> samples_noise{};
    std::transform(std::begin(n), std::end(n), std::back_inserter(samples_noise),
                   [&](auto const& n) {
        return envi::ecg(Ts * n) + 0.1 * std::sin(2.0 * M_PI * 50.0 * Ts * n) +
               0.1 * std::cos(2.0 * M_PI * 0.2 * Ts * n);
    });
    std::transform(std::begin(n), std::end(n), std::back_inserter(samples),
                   [&](auto const& n) {
        return envi::ecg(Ts * n);
    });

    // Direct Form II IIR System Second Order Sections
    std::vector<nerv::f64> w(envi::M + 1);
    std::vector<nerv::f64> y(envi::M + 1);
    std::vector<nerv::f64> output(sample_count);

    constexpr auto ra_size = 32;
    nerv::queue<nerv::f64, ra_size> ra{};

    // Simulate signal being read
    for (nerv::usize i = 0; i < sample_count; i++) {
        auto value = samples_noise[i];
        ra.enq(value);
        auto out = std::accumulate(std::rbegin(ra), std::rend(ra), 0.0, std::plus<nerv::f64>()) / nerv::f64(ra_size);
        output[i] = out;
    }

    std::ofstream data_file{"data.csv"};
    std::ofstream data_file_noise{"data_noise.csv"};
    std::ofstream data_file_out{"data_out.csv"};

    for (nerv::usize i = 0; i < n.size(); i++) {
        data_file       << n[i] << "," << samples[i]       << "\n";
        data_file_noise << n[i] << "," << samples_noise[i] << "\n";
        data_file_out   << n[i] << "," << output[i] << "\n";
    }

    return 0;
}

