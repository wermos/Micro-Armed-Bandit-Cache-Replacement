#include <algorithm>
#include <cassert>
#include <map>
#include <utility>

#include "cache.h"
#include "msl/fwcounter.h"

#include "replacement_policy.hpp"

namespace {
constexpr unsigned maxRRPV = 3;
constexpr std::size_t NUM_POLICY = 2;
constexpr std::size_t SDM_SIZE = 32;
constexpr std::size_t TOTAL_SDM_SETS = NUM_CPUS * NUM_POLICY * SDM_SIZE;
constexpr unsigned BIP_MAX = 32;
constexpr unsigned PSEL_WIDTH = 10;
}  // anonymous namespace

class DRRIP : ReplacementPolicy {
   public:
    DRRIP() = default;

    virtual void initialize(CACHE* cache_block) override {
        // randomly selected sampler sets
        std::size_t rand_seed = 1103515245 + 12345;

        for (std::size_t i = 0; i < ::TOTAL_SDM_SETS; i++) {
            std::size_t val = (rand_seed / 65536) % NUM_SET;
            auto loc = std::lower_bound(std::begin(rand_sets[cache_block]), std::end(rand_sets[cache_block]), val);

            while (loc != std::end(rand_sets[cache_block]) && *loc == val) {
                rand_seed = rand_seed * 1103515245 + 12345;
                val = (rand_seed / 65536) % cache_block->NUM_SET;
                loc = std::lower_bound(std::begin(rand_sets[cache_block]), std::end(rand_sets[cache_block]), val);
            }

            rand_sets[cache_block].insert(loc, val);
        }

        rrpv.insert({cache_block, std::vector<unsigned>(cache_block->NUM_SET * cache_block->NUM_WAY)});
    }

    virtual void updateState(CACHE* cache_block, std::uint64_t current_cycle, std::uint32_t triggering_cpu,
                             std::uint32_t set, std::uint32_t way, std::uint64_t full_addr, std::uint64_t ip,
                             std::uint64_t victim_addr, std::uint32_t type, std::uint8_t hit) override {
        // do not update replacement state for writebacks
        if (access_type{type} == access_type::WRITE) {
            rrpv[cache_block][set * cache_block->NUM_WAY + way] = ::maxRRPV - 1;
            return;
        }

        // cache hit
        if (hit) {
            rrpv[cache_block][set * cache_block->NUM_WAY + way] =
                0;  // for cache hit, DRRIP always promotes a cache line to the MRU position
            return;
        }

        // cache miss
        auto begin = std::next(std::begin(rand_sets[cache_block]), triggering_cpu * ::NUM_POLICY * ::SDM_SIZE);
        auto end = std::next(begin, ::NUM_POLICY * ::SDM_SIZE);
        auto leader = std::find(begin, end, set);

        if (leader == end) {  // follower sets
            auto selector = PSEL[std::make_pair(cache_block, triggering_cpu)];
            if (selector.value() > (selector.maximum / 2)) {  // follow BIP
                rrpv[cache_block][set * cache_block->NUM_WAY + way] = ::maxRRPV;

                bip_counter[cache_block]++;
                if (bip_counter[cache_block] == ::BIP_MAX) {
                    bip_counter[cache_block] = 0;
                    rrpv[cache_block][set * cache_block->NUM_WAY + way] = ::maxRRPV - 1;
                }
            } else {  // follow SRRIP
                rrpv[cache_block][set * cache_block->NUM_WAY + way] = ::maxRRPV - 1;
            }
        } else if (leader == begin) {  // leader 0: BIP
            PSEL[std::make_pair(cache_block, triggering_cpu)]--;
            rrpv[cache_block][set * cache_block->NUM_WAY + way] = ::maxRRPV;

            bip_counter[cache_block]++;
            if (bip_counter[cache_block] == ::BIP_MAX) {
                bip_counter[cache_block] = 0;
                rrpv[cache_block][set * cache_block->NUM_WAY + way] = ::maxRRPV - 1;
            }
        } else if (leader == std::next(begin)) {  // leader 1: SRRIP
            PSEL[std::make_pair(cache_block, triggering_cpu)]++;
            rrpv[cache_block][set * cache_block->NUM_WAY + way] = ::maxRRPV - 1;
        }
    }

    virtual std::uint32_t findVictim(CACHE* cache_block, std::uint64_t current_cycle, std::uint32_t triggering_cpu,
                                     std::uint64_t instr_id, std::uint32_t set, const CACHE::BLOCK* current_set,
                                     std::uint64_t ip, std::uint64_t full_addr, std::uint32_t type) override {
        // look for the maxRRPV line
        auto begin = std::next(std::begin(rrpv[cache_block]), set * cache_block->NUM_WAY);
        auto end = std::next(begin, NUM_WAY);

        auto victim = std::max_element(begin, end);
        for (auto it = begin; it != end; ++it)
            *it += ::maxRRPV - *victim;

        assert(begin <= victim);
        assert(victim < end);
        return static_cast<uint32_t>(std::distance(begin, victim));  // cast protected by assertions
    }

   private:
    std::map<CACHE*, unsigned> bip_counter;
    std::map<CACHE*, std::vector<std::size_t>> rand_sets;
    std::map<std::pair<CACHE*, std::size_t>, champsim::msl::fwcounter<PSEL_WIDTH>> PSEL;
    std::map<CACHE*, std::vector<unsigned>> rrpv;
};
