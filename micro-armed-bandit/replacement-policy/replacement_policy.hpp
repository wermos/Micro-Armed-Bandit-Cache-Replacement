#pragma once

#include <cstdint>

#include "cache.h"

class ReplacementPolicy {
    virtual void initialize(CACHE*) = 0;
    virtual void updateState(CACHE* cache_block, std::uint64_t current_cycle, std::uint32_t triggering_cpu,
                             std::uint32_t set, std::uint32_t way, std::uint64_t full_addr, std::uint64_t ip,
                             std::uint64_t victim_addr, std::uint32_t type, std::uint8_t hit) = 0;
    virtual std::uint32_t findVictim(CACHE* cache_block, std::uint64_t current_cycle, std::uint32_t triggering_cpu,
                                     std::uint64_t instr_id, std::uint32_t set, const CACHE::BLOCK* current_set,
                                     std::uint64_t ip, std::uint64_t full_addr, std::uint32_t type) = 0;
};