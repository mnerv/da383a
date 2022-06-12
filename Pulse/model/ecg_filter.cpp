/**
 * @file   ecg_filter.hpp
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
#include <array>
#include <random>

#include "types.hpp"
#include "ring.hpp"

namespace env {
auto clear = "\033[H\033[2J";
auto hide  = "\033[?25l";
auto show  = "\033[?25h";
auto reset = "\u001b[0m";


template <typename T, std::size_t N>
constexpr auto length_of(T (&)[N]) -> std::size_t {
    return N;
}

auto ecg(nrv::f64 x) -> nrv::f64 {
    auto f = 1.5;
    return std::sin(2.0 * M_PI * f * 4.0 * x) *
           std::pow(0.5 * (std::sin(2.0 * M_PI * f * x) + 1.0), 5.0);
}

auto test(nrv::f64 x) -> nrv::f64 {
    return std::sin(2.0 * M_PI * 125.0 * x);
}

auto iir_high_pass(nrv::f64 const& value) {
    // [Hz] Butterworth
    // Fs    = 1000
    // Fstop = 0.1
    // Fpass = 0.8
    // [dB]
    // Astop = 80
    // Apass = 1
    static nrv::f64 b[] = {
         0.9936059630099,    -4.96802981505,    9.936059630099,   -9.936059630099,
           4.96802981505,  -0.9936059630099
    };
    static nrv::f64 a[] = {
                       1,   -4.987170880032,     9.94876577478,   -9.923271718397,
          4.948929633379,  -0.9872528097289
    };
    static nrv::ring<nrv::f64, length_of(b)> x_r{};
    static nrv::ring<nrv::f64, length_of(a)> y_r{};

    x_r.enq(value);

    auto forward = 0.0;
    for (std::size_t i = 0; i < x_r.capacity(); i++) {
        forward += b[i] * x_r[i];
    }

    auto feedback = 0.0;
    for (std::size_t i = 1; i < y_r.capacity(); i++) {
        feedback += -a[i] * y_r[i - 1];
    }
    y_r.enq(forward + feedback);
    return *y_r.rbegin();
}

auto iir_low_pass(nrv::f64 const& value) {
    // [Hz] Butterworth
    // Fs    = 1000
    // Fpass = 5
    // Fstop = 30
    // [dB]
    // Astop = 80
    // Apass = 1
    static nrv::f64 b[] = {
  6.594578622361e-11,3.956747173417e-10,9.891867933541e-10,1.318915724472e-09,
  9.891867933541e-10,3.956747173417e-10,6.594578622361e-11
    };
    static nrv::f64 a[] = {
                   1,   -5.842652126594,    14.22559205773,   -18.47523699553,
      13.49869991173,   -5.260796021795,   0.8543931786795
    };
    static nrv::ring<nrv::f64, length_of(b)> x_r{};
    static nrv::ring<nrv::f64, length_of(a)> y_r{};

    x_r.enq(value);

    auto forward = 0.0;
    for (std::size_t i = 0; i < x_r.capacity(); i++) {
        forward += b[i] * x_r[i];
    }

    auto feedback = 0.0;
    for (std::size_t i = 1; i < y_r.capacity(); i++) {
        feedback += -a[i] * y_r[i - 1];
    }
    y_r.enq(forward + feedback);
    return *y_r.rbegin();
}

}  // namespace env

auto main([[maybe_unused]]nrv::i32 argc, [[maybe_unused]]char const* argv[]) -> nrv::i32 {
    constexpr auto fs = 1'000.0;
    constexpr auto Ts = 1.0 / fs;
    constexpr nrv::usize sample_count = 6000;

    std::vector<nrv::f64> n{};
    std::generate_n(std::back_inserter(n), sample_count, [i = 0.0]() mutable { return i++;});

    //std::random_device rdev{};
    //std::mt19937 rng{rdev()};
    std::mt19937 rng{0};
    std::uniform_real_distribution<nrv::f64> dist(-1.0, 1.0);
    std::uniform_real_distribution<nrv::f64> dist_norm(0.0, 0.5);

    std::vector<nrv::f64> samples{};
    std::vector<nrv::f64> samples_noise{};
    std::transform(std::begin(n), std::end(n), std::back_inserter(samples),
                   [&](auto const& n) {
        //return env::test(Ts * n);
        //return env::ecg(Ts * n);
        return std::sin(2.0 * 2.0 * M_PI * Ts * n);
    });
    //auto noise = [](nrv::f64 n) {
    //    return 0.4 * std::sin(2.0 * M_PI * 500.0 * n * Ts)
    //         + 0.1 * std::cos(2.0 * M_PI * 444.0 * n * Ts + M_PI / 5.0);
    //};
    auto ecg_noise = [&](nrv::f64 n) {
        return 0.1 * std::sin(2.0 * M_PI * 55.0 * Ts * n) +
               0.5 * std::cos(2.0 * M_PI * 0.1 * Ts * n);
               //+ 0.2 * dist(rng) + dist_norm(rng) * dist(rng);
    };
    std::transform(std::begin(n), std::end(n),
                   std::back_inserter(samples_noise), [&](auto const& n) mutable {
        //return env::test(Ts * n) + noise(n);
        //return env::test(Ts * n) + 0.1 * std::sin(2.0 * M_PI * 50.0 * Ts * n) + 0.1 * std::cos(2.0 * M_PI * 1000.0 * Ts * n);
        //return env::ecg(Ts * n) + ecg_noise(n);
        return std::sin(2.0 * 2.0 * M_PI * Ts * n) + ecg_noise(n);
    });

    // Direct Form II IIR System Second Order Sections

    std::vector<nrv::f64> output(sample_count);
    //env::ring_t w{};

    for (nrv::usize i = 0; i < sample_count; i++) {
        auto read_value = samples_noise[i];
        output[i] = env::iir_high_pass(read_value);
        output[i] = env::iir_low_pass(output[i]);
        //std::cout << output[i] << "\n";
        //output[i] = read_value;
    }

    std::ofstream plot_data{"plot_data.csv"};
    plot_data << "samples" << "," << "original" << "," << "w/ noise" << "," << "filtered" << "\n";
    for (nrv::usize i = 0; i < sample_count; i++) {
        plot_data << n[i]       << ",";
        plot_data << samples[i] << ",";
        plot_data << samples_noise[i] << ",";
        plot_data << output[i]        << "\n";
    }

    std::cout << "IIR filter\n";
    return 0;
}

