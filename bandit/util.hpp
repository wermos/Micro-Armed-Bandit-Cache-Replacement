#pragma once

#include <cstddef>
#include <limits>
#include <random>

inline double randomDouble() {
    // Returns a random real in [0, 1].
    static std::uniform_real_distribution<double> distribution(
        0.0, std::nextafter(1.0, std::numeric_limits<double>::infinity()));
    static std::mt19937_64 generator;

    return distribution(generator);
}

inline std::size_t randomInteger(std::size_t N) {
    // Returns a random integer in {1, 2, 3, ..., N}.
    static std::uniform_int_distribution<std::size_t> distribution(1, N);
    static std::mt19937_64 generator;

    return distribution(generator);
}
