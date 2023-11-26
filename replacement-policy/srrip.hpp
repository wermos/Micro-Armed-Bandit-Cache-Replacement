#include <cassert>
#include <unordered_map>

#include "cache.h"
#include "constants.hpp"
#include "replacement_policy.hpp"

class SRRIP : public ReplacementPolicy {
   public:
    SRRIP() = default;

    virtual void initialize(CACHE* cache_block) override {
        rrpv_values[cache_block] = std::vector<int>(cache_block->NUM_SET * cache_block->NUM_WAY, ::maxRRPV);
    }

    virtual void updateState(CACHE* cache_block, std::uint64_t current_cycle, std::uint32_t triggering_cpu,
                             std::uint32_t set, std::uint32_t way, std::uint64_t full_addr, std::uint64_t ip,
                             std::uint64_t victim_addr, std::uint32_t type, std::uint8_t hit) override {
        if (hit)
            rrpv_values[cache_block][set * cache_block->NUM_WAY + way] = 0;
        else
            rrpv_values[cache_block][set * cache_block->NUM_WAY + way] = ::maxRRPV - 1;
    }

    virtual std::uint32_t findVictim(CACHE* cache_block, std::uint64_t current_cycle, std::uint32_t triggering_cpu,
                                     std::uint64_t instr_id, std::uint32_t set, const CACHE::BLOCK* current_set,
                                     std::uint64_t ip, std::uint64_t full_addr, std::uint32_t type) override {
        // look for the maxRRPV line
        auto begin = std::next(std::begin(rrpv_values[cache_block]), set * cache_block->NUM_WAY);
        auto end = std::next(begin, cache_block->NUM_WAY);
        auto victim = std::find(begin, end, ::maxRRPV);  // hijack the lru field
        while (victim == end) {
            for (auto it = begin; it != end; ++it)
                ++(*it);

            victim = std::find(begin, end, ::maxRRPV);
        }

        assert(begin <= victim);
        assert(victim < end);
        return static_cast<std::uint32_t>(std::distance(begin, victim));  // cast protected by assertions
    }

   private:
    std::unordered_map<CACHE*, std::vector<int>> rrpv_values;
};
