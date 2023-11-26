#include <algorithm>
#include <cassert>
#include <cstdint>
#include <map>
#include <vector>

#include "cache.h"
#include "replacement_policy.hpp"

class LRU : public ReplacementPolicy {
   public:
    LRU() = default;

    virtual void initialize(CACHE* cache_block) override {
        last_used_cycles[cache_block] = std::vector<uint64_t>(cache_block->NUM_SET * cache_block->NUM_WAY);
    }

    virtual void updateState(CACHE* cache_block, std::uint64_t current_cycle, std::uint32_t triggering_cpu,
                             std::uint32_t set, std::uint32_t way, std::uint64_t full_addr, std::uint64_t ip,
                             std::uint64_t victim_addr, std::uint32_t type, std::uint8_t hit) override {
        // Mark the way as being used on the current cycle
        if (!hit || access_type{type} != access_type::WRITE) {
            // Skip this for writeback hits
            last_used_cycles[cache_block].at(set * cache_block->NUM_WAY + way) = current_cycle;
        }
    }

    virtual std::uint32_t findVictim(CACHE* cache_block, std::uint64_t current_cycle, std::uint32_t triggering_cpu,
                                     std::uint64_t instr_id, std::uint32_t set, const CACHE::BLOCK* current_set,
                                     std::uint64_t ip, std::uint64_t full_addr, std::uint32_t type) override {
        auto begin = std::next(std::begin(last_used_cycles[cache_block]), set * cache_block->NUM_WAY);
        auto end = std::next(begin, cache_block->NUM_WAY);

        // Find the way whose last use cycle is most distant
        auto victim = std::min_element(begin, end);
        assert(begin <= victim);
        assert(victim < end);
        return static_cast<std::uint32_t>(std::distance(begin, victim));  // cast protected by prior asserts
    }

   private:
    std::map<CACHE*, std::vector<std::uint64_t>> last_used_cycles;
};
