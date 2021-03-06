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
#include <array>
#include <random>

#include "types.hpp"
#include "ring.hpp"

namespace env {
auto clear = "\033[H\033[2J";
auto hide  = "\033[?25l";
auto show  = "\033[?25h";
auto reset = "\u001b[0m";

nrv::f64 a[] = {
1,-6.70218347938014,24.2633638293347,-59.0054321458422,106.180528967321,-147.266003327997,161.009487809362,-139.436308280412,95.1865660901505,-50.0780390785878,19.4940629397266,-5.09719247659637,0.720189367854421
};
nrv::f64 b[] = {
0.00148036694850925,-0.00860932170753858,0.0273399672684535,-0.0600063877173575,0.100704056886858,-0.135176869970052,0.148805416797188,-0.135176869970052,0.100704056886859,-0.0600063877173578,0.0273399672684536,-0.00860932170753864,0.00148036694850926
};

template <typename T, std::size_t N>
constexpr auto length_of(T (&)[N]) -> std::size_t {
    return N;
}

using ring_t = nrv::ring<nrv::f64, length_of(b)>;

auto ecg(nrv::f64 x) -> nrv::f64 {
    auto f = 1.5;
    return std::sin(2.0 * M_PI * f * 4.0 * x) *
           std::pow(0.5 * (std::sin(2.0 * M_PI * f * x) + 1.0), 5.0);
}

auto test(nrv::f64 x) -> nrv::f64 {
    return std::sin(2.0 * M_PI * 150.0 * x);
}

static nrv::ring<nrv::f64, length_of(b)> x_r{};
static nrv::ring<nrv::f64, length_of(a)> y_r{};

auto iir(nrv::f64 const& value) -> nrv::f64 {
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
    constexpr nrv::usize sample_count = 2048;

    std::vector<nrv::f64> n{};
    std::generate_n(std::back_inserter(n), sample_count, [i = 0.0]() mutable { return i++;});

    //std::random_device rdev{};
    //std::mt19937 rng{rdev()};
    std::mt19937 rng{0};
    std::uniform_real_distribution<nrv::f64> dist(-1.0, 1.0);

    std::vector<nrv::f64> samples{};
    std::vector<nrv::f64> samples_noise{};
    std::transform(std::begin(n), std::end(n), std::back_inserter(samples),
                   [&](auto const& n) {
        return env::test(Ts * n);
    });
    auto noise = [](nrv::f64 n) {
        return 0.4 * std::sin(2.0 * M_PI * 500.0 * n * Ts)
             + 0.1 * std::cos(2.0 * M_PI * 444.0 * n * Ts + M_PI / 5.0);
    };
    std::transform(std::begin(n), std::end(n),
                   std::back_inserter(samples_noise), [&](auto const& n) mutable {
        return env::test(Ts * n) + noise(n);
    });

    // Direct Form II IIR System Second Order Sections

    std::vector<nrv::f64> output(sample_count);
    //env::ring_t w{};

    for (nrv::usize i = 0; i < sample_count; i++) {
        auto read_value = samples_noise[i];
        output[i] = env::iir(read_value);
        //std::cout << output[i] << "\n";
    }

    std::ofstream plot_data{"plot_data.csv"};
    plot_data << "samples" << "," << "original" << "," << "w/ noise" << "," << "filtered" << "\n";
    for (nrv::usize i = 1024; i < sample_count; i++) {
        plot_data << n[i]       << ",";
        plot_data << samples[i] << ",";
        plot_data << samples_noise[i] << ",";
        plot_data << output[i]        << "\n";
    }

    std::cout << "IIR filter\n";
    return 0;
}

