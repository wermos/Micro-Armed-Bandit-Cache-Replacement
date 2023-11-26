#pragma once

#include <cstdint>
#include <memory>

#include "bandit/multi_armed_bandit.hpp"
#include "bandit/policy.hpp"

#include "replacement-policy/replacement_policy.hpp"
#include "replacement-policy/drrip.hpp"
#include "replacement-policy/lru.hpp"
#include "replacement-policy/ship.hpp"
#include "replacement-policy/srrip.hpp"

class Orchestrator {
   public:
    Orchestrator(std::size_t N) : N{N}, c{c}, gamma{gamma}, curr_policy{0} {
        DUCB* policy = new DUCB(N, c, gamma);
        bandit = MultiArmedBandit(N, policy);

        replacementPolicy.push_back(std::make_unique<LRU>());
        replacementPolicy.push_back(std::make_unique<DRRIP>());
        replacementPolicy.push_back(std::make_unique<SHIP>()));
        replacementPolicy.push_back(std::make_unique<SRRIP>());
    }

    void initialize_replacement() {
        for (auto policy : replacementPolicy) {
            policy->initialize();
        }
    }

    std::uint32_t findVictim(CACHE* cache_block, std::uint64_t current_cycle, std::uint32_t triggering_cpu,
                             std::uint64_t instr_id, std::uint32_t set, const CACHE::BLOCK* current_set,
                             std::uint64_t ip, std::uint64_t full_addr, std::uint32_t type) override {
        replacementPolicy[curr_policy]->findVictim(cache_block, current_cycle, triggering_cpu, instr_id, set, current_set, ip, full_addr, type);
    }

    void updateState(CACHE* cache_block, std::uint64_t current_cycle, std::uint32_t triggering_cpu,
                     std::uint32_t set, std::uint32_t way, std::uint64_t full_addr, std::uint64_t ip,
                     std::uint64_t victim_addr, std::uint32_t type, std::uint8_t hit)
        for (auto policy : replacementPolicy) {
            policy->updateState(cache_block, current_cycle, triggering_cpu, set, way, full_addr, ip, victim_addr, type, hit);
        }
    }

   private:
    std::size_t N;
    double c;
    double gamma;
    MultiArmedBandit bandit;
    std::vector<std::unique_ptr<ReplacementPolicy>> replacementPolicy;
    curr_policy;
};
