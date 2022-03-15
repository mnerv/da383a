#include <iostream>
#include <cstdint>
#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include <algorithm>
#include <numeric>
#include <fstream>
#include <complex>

auto print_vector(std::vector<double> const& values) -> void {
    std::for_each(std::begin(values), std::end(values), [](auto const& f) {
        std::cout << f << "\n";
    });
}

auto fourier_transform(double const& k, std::vector<double> const& samples) -> std::complex<double> {
    using namespace std::complex_literals;
    auto N = double(samples.size());
    return std::accumulate(std::begin(samples), std::end(samples), std::complex<double>{},
    [&, n = 0.0](auto const& a, auto const& b) mutable {
        // apply Euler's formula
        auto real = std::cos((-2.0 * M_PI * k * n) / N);
        auto img  = std::sin((-2.0 * M_PI * k * n) / N);

        n++; return a + b * std::complex<double>{real, img};
    });
}

auto dft(std::vector<double> const& F, std::vector<double> const& samples) -> std::vector<std::complex<double>> {
    std::vector<std::complex<double>> frequencies;
    std::transform(std::begin(F), std::end(F), std::back_inserter(frequencies),
        [&](auto const& k) {
        return fourier_transform(k, samples);
    });
    return frequencies;
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
    auto dft_out = dft(F, samples);
    std::vector<double> dft_abs{};
    std::transform(std::begin(dft_out), std::end(dft_out), std::back_inserter(dft_abs),
                   [](auto const& value) {
        return std::abs(value);
    });

    std::vector<double> dft_nyquist{};
    std::copy_if(std::begin(dft_abs), std::end(dft_abs), std::back_inserter(dft_nyquist),
        [&, i = 0](auto const&) mutable {
        return F[i++] < (fs / 2.0);
    });
    std::transform(std::begin(dft_nyquist), std::end(dft_nyquist), std::begin(dft_nyquist),
                   [&samples](auto const& a){
        auto N = samples.size();
        return (a * 2.0) / N;
    });

    // write data as csv for plot
    std::ofstream sample_file{"sample_data.csv"};
    for (std::size_t i = 0; i < n.size(); i++) {
        sample_file << n[i] << "," << samples[i] << "\n";
    }
    sample_file.close();

    std::ofstream dft_file{"dft_data.csv"};
    for (std::size_t i = 0; i < F.size(); i++) {
        if (F[i] < (fs / 2.0))
            dft_file << F[i] << "," << dft_nyquist[i] << "\n";
    }
    dft_file.close();

    return 0;
}

