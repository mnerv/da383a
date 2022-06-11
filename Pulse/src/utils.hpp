/**
 * @file   utils.hpp
 * @author Pratchaya Khansomboon (pratchaya.k.git@gmail.com)
 * @brief  Collection of utility functions
 * @date   2022-03-21
 *
 * @copyright Copyright (c) 2022
 */
#pragma once
#include <type_traits>

namespace nrv {
template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
auto map(T const& x, T const& in_min, T const& in_max, T const& out_min, T const& out_max) -> T {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
}

