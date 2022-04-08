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
//#include "fdacoefs_ss.h"

namespace envi {
auto clear = "\033[H\033[2J";
auto hide  = "\033[?25l";
auto show  = "\033[?25h";
auto reset = "\u001b[0m";

using queue_t = nerv::queue<nerv::f64, 3>;

auto ecg(nerv::f64 x) -> nerv::f64 {
    auto f = 1.5;
    return std::sin(2.0 * M_PI * f * 4.0 * x) *
           std::pow(0.5 * (std::sin(2.0 * M_PI * f * x) + 1.0), 5.0);
}

auto test(nerv::f64 x) -> nerv::f64 {
    return std::sin(2.0 * M_PI * 125.0 * x);
}

auto iir(queue_t& w, nerv::f64 x) -> nerv::f64 {
    nerv::f64 output = x;
    for (nerv::usize i = 0; i < MWSPT_NSEC - 1; i += 2) {
        auto a = DEN[i + 1];
        auto b = NUM[i + 1];
        nerv::f64 feedback = output * NUM[i][0];
        for (nerv::usize j = 1; j < w.capacity(); j++) {
            feedback += -a[j] * w.at_back(j);
        }
        w.enq(feedback);
        nerv::f64 forward  = 0.0;
        for (nerv::usize j = 0; j < w.capacity(); j++) {
            forward += b[j] * w.at_front(j);
        }
        output = forward;
    }
    return output * NUM[MWSPT_NSEC - 1][0];
}
}

auto main([[maybe_unused]]nerv::i32 argc, [[maybe_unused]]char const* argv[]) -> nerv::i32 {
    constexpr auto fs = 10'000.;
    constexpr auto Ts = 1.0 / fs;
    constexpr nerv::usize sample_count = 500;

    std::vector<nerv::f64> n{};
    std::generate_n(std::back_inserter(n), sample_count, [i = 0.0]() mutable { return i++;});

    std::vector<nerv::f64> samples{};
    std::vector<nerv::f64> samples_noise{};
    std::transform(std::begin(n), std::end(n), std::back_inserter(samples),
                   [&](auto const& n) {
        //return envi::ecg(Ts * n) + 0.1 * std::sin(2.0 * M_PI * 50.0 * Ts * n) +
        //       0.1 * std::cos(2.0 * M_PI * 0.2 * Ts * n);
        //return envi::test(Ts * n) +
        //   0.2 * std::sin(2.0 * M_PI * 1000.0 * Ts * n) +
        //   0.15 * std::sin(2.0 * M_PI * 50.0 * Ts * n);
        return std::sin(2.0 * M_PI * Ts * n * 650.0);
    });
    std::transform(std::begin(samples), std::end(samples),
                   std::back_inserter(samples_noise), [&, n = 0](auto const& s) mutable {
        return s + std::sin(2.0 * M_PI * Ts * (n++) * 45.0) * 0.5;
    });

    // Direct Form II IIR System Second Order Sections

    std::vector<nerv::f64> output(sample_count);
    nerv::queue<nerv::f64, 3> w{};

    // Simulate signal being read
    //auto read_value = samples_noise[1];
    //output[0] = envi::iir(w, read_value);
    for (nerv::usize i = 0; i < sample_count; i++) {
        auto read_value = samples_noise[i];
        output[i] = envi::iir(w, read_value);
        //std::cout << output[i] << "\n";
    }

    std::ofstream plot_data{"plot_data.csv"};
    plot_data << "samples" << "," << "original" << "," << "w/ noise" << "," << "filtered" << "\n";
    for (nerv::usize i = 0; i < n.size(); i++) {
        plot_data << n[i]       << ",";
        plot_data << samples[i] << ",";
        plot_data << samples_noise[i] << ",";
        plot_data << output[i]        << "\n";
    }

    std::cout << "IIR filter\n";
    return 0;
}

