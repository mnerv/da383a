#include <iostream>
#include <cstdint>
#include <vector>
#include <numeric>
#include <string>

namespace env {
auto reverse_bit(std::size_t b, std::size_t bit_size) -> std::size_t {
    std::size_t n = 0;
    for (std::size_t i = 0; i < bit_size; ++i) {
        n = n << 1;
        n = n | (b & 1);
        b = b >> 1;
    }
    return n;
}

auto parse_usize(std::string const& str, std::size_t default_num = 8) -> std::size_t {
    try {
        return std::stoul(str);
    } catch (std::invalid_argument const& e) {
        return default_num;
    }
}

auto is_power2(std::size_t n) -> bool {
    return (n != 0) && (n & (n - 1)) == 0;
}
}

auto main([[maybe_unused]]std::int32_t argc, [[maybe_unused]]char const* argv[]) -> std::int32_t {
    if (argc < 2) {
        std::cerr << "error: not enough arguments" << "\n";
        return 1;
    }

    constexpr std::size_t DEFAULT_SIZE = 8;
    auto size = env::parse_usize(argv[1], DEFAULT_SIZE);

    if (!env::is_power2(size)) {
        std::cerr << "error: size needs to be power of 2!" << "\n";
        return 1;
    }

    std::vector<std::size_t> n{};
    std::generate_n(std::back_inserter(n), size, [i = 0]() mutable { return i++; });

    std::vector<std::size_t> rn{};
    std::size_t const bit_size = std::log(size) / std::log(2);  // log_2(size)
    std::transform(std::begin(n), std::end(n), std::back_inserter(rn),
        [bit_size](auto const& n) {
        return env::reverse_bit(n, bit_size);
    });

    for (std::size_t i = 0; i < size; ++i) {
        std::cout << n[i] << "    " << rn[i] << "\n";
    }

    return 0;
}

