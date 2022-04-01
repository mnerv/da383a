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
#include "queue.hpp"

#include "fdacoefs.h"

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

auto test(nerv::f64 x) -> nerv::f64 {
    return std::sin(2.0 * M_PI * 125.0 * x);
}

auto iir() -> nerv::f64 {
    [[maybe_unused]]nerv::f64 feedback = 0.0;
    [[maybe_unused]]nerv::f64 forward  = 0.0;
    return 0.0;
}
}

auto main([[maybe_unused]]nerv::i32 argc, [[maybe_unused]]char const* argv[]) -> nerv::i32 {
    constexpr auto fs = 5'000.;
    constexpr auto Ts = 1.0 / fs;
    constexpr std::size_t sample_count = 250;

    std::vector<nerv::f64> n{};
    std::generate_n(std::back_inserter(n), sample_count, [i = 0.0]() mutable { return i++;});

    std::vector<nerv::f64> samples{};
    std::vector<nerv::f64> samples_noise{};
    std::transform(std::begin(n), std::end(n), std::back_inserter(samples_noise),
                   [&](auto const& n) {
        //return envi::ecg(Ts * n) + 0.1 * std::sin(2.0 * M_PI * 50.0 * Ts * n) +
        //       0.1 * std::cos(2.0 * M_PI * 0.2 * Ts * n);
        return envi::test(Ts * n) +
           0.2 * std::sin(2.0 * M_PI * 1000.0 * Ts * n) +
           0.15 * std::sin(2.0 * M_PI * 50.0 * Ts * n);
    });
    std::transform(std::begin(n), std::end(n), std::back_inserter(samples),
                   [&](auto const& n) {
        return envi::test(Ts * n);
    });

    // Direct Form II IIR System Second Order Sections

    std::vector<nerv::f64> output(sample_count);

    // Simulate signal being read
    for (nerv::usize i = 0; i < sample_count; i++) {
        [[maybe_unused]]auto read_value = samples_noise[i];
        output[i] = 0.0;
    }

    std::ofstream plot_data{"plot_data.csv"};
    plot_data << "samples" << "," << "original" << "," << "w/ noise" << "," << "filtered" << "\n";
    for (nerv::usize i = 0; i < n.size(); i++) {
        plot_data << n[i]       << ",";
        plot_data << samples[i] << ",";
        plot_data << samples_noise[i] << ",";
        plot_data << output[i]        << "\n";
    }

    return 0;
}

