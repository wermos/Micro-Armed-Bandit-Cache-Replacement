#include <algorithm>
#include <array>
#include <cassert>
#include <map>
#include <utility>
#include <vector>

#include "cache.h"
#include "msl/bits.h"

#include "replacement_policy.hpp"

namespace {
constexpr int maxRRPV = 3;
constexpr std::size_t SHCT_SIZE = 16384;
constexpr unsigned SHCT_PRIME = 16381;
constexpr std::size_t SAMPLER_SET = (256 * NUM_CPUS);
constexpr unsigned SHCT_MAX = 7;
}  // anonymous namespace

// sampler structure
class SAMPLER_class {
   public:
    bool valid = false;
    uint8_t used = 0;
    uint64_t address = 0, cl_addr = 0, ip = 0;
    uint64_t last_used = 0;
};

class SHIP : ReplacementPolicy {
   public:
    SHIP() = default;

    virtual void initialize(CACHE* cache_block) override {
        // randomly selected sampler sets
        std::size_t rand_seed = 1103515245 + 12345;

        for (std::size_t i = 0; i < ::SAMPLER_SET; i++) {
            std::size_t val = (rand_seed / 65536) % cache_block->NUM_SET;
            std::vector<std::size_t>::iterator loc =
                std::lower_bound(std::begin(rand_sets[cache_block]), std::end(rand_sets[cache_block]), val);

            while (loc != std::end(rand_sets[cache_block]) && *loc == val) {
                rand_seed = rand_seed * 1103515245 + 12345;
                val = (rand_seed / 65536) % NUM_SET;
                loc = std::lower_bound(std::begin(rand_sets[cache_block]), std::end(rand_sets[cache_block]), val);
            }

            rand_sets[cache_block].insert(loc, val);
        }

        sampler.emplace(cache_block, ::SAMPLER_SET * cache_block->NUM_WAY);

        rrpv_values[cache_block] = std::vector<int>(cache_block->NUM_SET * cache_block->NUM_WAY, ::maxRRPV);
    }

    virtual void updateState(CACHE* cache_block, std::uint64_t current_cycle, std::uint32_t triggering_cpu,
                             std::uint32_t set, std::uint32_t way, std::uint64_t full_addr, std::uint64_t ip,
                             std::uint64_t victim_addr, std::uint32_t type, std::uint8_t hit) override {
        // handle writeback access
        if (access_type{type} == access_type::WRITE) {
            if (!hit)
                rrpv_values[cache_block][set * cache_block->NUM_WAY + way] = ::maxRRPV - 1;

            return;
        }

        // update sampler
        auto s_idx = std::find(std::begin(rand_sets[cache_block]), std::end(rand_sets[cache_block]), set);
        if (s_idx != std::end(rand_sets[cache_block])) {
            auto s_set_begin =
                std::next(std::begin(sampler[cache_block]), std::distance(std::begin(rand_sets[cache_block]), s_idx));
            auto s_set_end = std::next(s_set_begin, cache_block->NUM_WAY);

            // check hit
            auto match = std::find_if(s_set_begin, s_set_end, addr = full_addr, shamt = 8 + champsim::lg2(cache_block->NUM_WAY)](auto x) {
                return x.valid && (x.address >> shamt) == (addr >> shamt); });

            if (match != s_set_end) {
                auto SHCT_idx = match->ip % ::SHCT_PRIME;
                if (SHCT[std::make_pair(cache_block, triggering_cpu)][SHCT_idx] > 0)
                    SHCT[std::make_pair(cache_block, triggering_cpu)][SHCT_idx]--;

                match->used = 1;
            } else {
                match =
                    std::min_element(s_set_begin, s_set_end, [](auto x, auto y) { return x.last_used < y.last_used; });

                if (match->used) {
                    auto SHCT_idx = match->ip % ::SHCT_PRIME;
                    if (SHCT[std::make_pair(cache_block, triggering_cpu)][SHCT_idx] < ::SHCT_MAX)
                        SHCT[std::make_pair(cache_block, triggering_cpu)][SHCT_idx]++;
                }

                match->valid = 1;
                match->address = full_addr;
                match->ip = ip;
                match->used = 0;
            }

            // update LRU state
            match->last_used = current_cycle;
        }

        if (hit)
            rrpv_values[cache_block][set * cache_block->NUM_WAY + way] = 0;
        else {
            // SHIP prediction
            auto SHCT_idx = ip % ::SHCT_PRIME;

            rrpv_values[cache_block][set * cache_block->NUM_WAY + way] = ::maxRRPV - 1;

            if (SHCT[std::make_pair(cache_block, triggering_cpu)][SHCT_idx] == ::SHCT_MAX)
                rrpv_values[cache_block][set * cache_block->NUM_WAY + way] = ::maxRRPV;
        }
    }

    virtual std::uint32_t findVictim(CACHE* cache_block, std::uint64_t current_cycle, std::uint32_t triggering_cpu,
                                     std::uint64_t instr_id, std::uint32_t set, const CACHE::BLOCK* current_set,
                                     std::uint64_t ip, std::uint64_t full_addr, std::uint32_t type) override {
        // look for the maxRRPV line
        auto begin = std::next(std::begin(rrpv_values[cache_block]), set * cache_block->NUM_WAY);
        auto end = std::next(begin, cache_block->NUM_WAY);
        auto victim = std::find(begin, end, ::maxRRPV);
        while (victim == end) {
            for (auto it = begin; it != end; ++it)
                ++(*it);

            victim = std::find(begin, end, ::maxRRPV);
        }

        assert(begin <= victim);
        return static_cast<std::uint32_t>(std::distance(begin, victim));  // cast pretected by prior assert
    }

   private:
    // sampler
    std::map<CACHE*, std::vector<std::size_t>> rand_sets;
    std::map<CACHE*, std::vector<SAMPLER_class>> sampler;
    std::map<CACHE*, std::vector<int>> rrpv_values;

    // prediction table structure
    std::map<std::pair<CACHE*, std::size_t>, std::array<unsigned, SHCT_SIZE>> SHCT;
};
