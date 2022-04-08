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

//#include "fdacoefs.h"
#include "fdacoefs_ss.h"

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

auto iir([[maybe_unused]]queue_t& w, nerv::f64 x) -> nerv::f64 {
    //static nerv::usize n = 0;
    //auto a = DEN;
    //auto b = NUM;
    //constexpr nerv::usize M = NL - 1;
    //constexpr nerv::usize N = DL - 1;
    //static nerv::f64 XX[M + 1];
    //static nerv::f64 YY[N + 1];

    //XX[n] = x;
    //auto sum_a = 0.0f;
    //for (nerv::usize i = 0; i < M + 1; i++) {
    //    auto index = (n + (M + 1) - i) % (M + 1);  // [n - k]
    //    sum_a += b[i] * XX[index];
    //}

    //auto sum_b = 0.0f;
    //for (nerv::usize i = 1; i < N + 1; i++) {
    //    auto index = (n + (N + 1) - i) % (N + 1);  // [n - l]
    //    sum_b += -a[i] * YY[index];
    //}
    //auto sum = sum_a + sum_b;
    //YY[n] = sum;
    //n = (n + 1) % (M + 1);

    //return sum;
    auto a = DEN;
    auto b = NUM;
    constexpr nerv::usize M = NL - 1;
    constexpr nerv::usize N = DL - 1;
    static nerv::queue<nerv::f64, M + 1> XX{};
    static nerv::queue<nerv::f64, N + 1> YY{};

    XX.enq(x);
    auto sum_a = 0.0f;
    for (nerv::usize i = 0; i < M + 1; i++) {
        sum_a += b[i] * XX.at_front(i);
    }

    auto sum_b = 0.0f;
    for (nerv::usize i = 1; i < N + 1; i++) {
        sum_b += -a[i] * YY.at_back(i);
    }
    auto sum = sum_a + sum_b;
    YY.enq(sum);

    return sum;

    //for (nerv::usize i = 0; i < MWSPT_NSEC - 1; i += 2) {
    //    auto a = DEN[i + 1];
    //    auto b = NUM[i + 1];

    //    nerv::f64 feedback = x * NUM[i][0];
    //    for (nerv::usize j = 1; j < DL[i + 1]; j++) {
    //        feedback += a[j] * w.at_front(j);
    //    }
    //    w.enq(feedback);
    //    nerv::f64 forward  = 0.0;
    //    for (nerv::usize j = 0; j < NL[i + 1]; j++) {
    //        forward += b[j] * w.at_back(j);
    //    }
    //    x = forward;
    //}
    //return x * NUM[MWSPT_NSEC - 1][0];
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
    for (nerv::usize i = 100; i < n.size(); i++) {
        plot_data << n[i]       << ",";
        plot_data << samples[i] << ",";
        plot_data << samples_noise[i] << ",";
        plot_data << output[i]        << "\n";
    }

    std::cout << "IIR filter\n";
    return 0;
}

