#pragma once

constexpr int maxRRPV = 3;
constexpr std::size_t SHCT_SIZE = 16384;
constexpr unsigned SHCT_PRIME = 16381;
constexpr std::size_t SAMPLER_SET = (256 * NUM_CPUS);
constexpr unsigned SHCT_MAX = 7;

constexpr std::size_t NUM_POLICY = 2;
constexpr std::size_t SDM_SIZE = 32;
constexpr std::size_t TOTAL_SDM_SETS = NUM_CPUS * NUM_POLICY * SDM_SIZE;
constexpr unsigned BIP_MAX = 32;
constexpr unsigned PSEL_WIDTH = 10;
