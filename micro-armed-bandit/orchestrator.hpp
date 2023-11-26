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
    Orchestrator(std::size_t N) : N{N}, c{0.01}, gamma{0.975}, currentPolicy{0}, nextUpdateCycle{0} {
        DUCB* policy = new DUCB(N, c, gamma);
        bandit = MultiArmedBandit(N, policy);

        replacementPolicy.push_back(std::make_unique<LRU>());
        replacementPolicy.push_back(std::make_unique<DRRIP>());
        replacementPolicy.push_back(std::make_unique<SHIP>()));
        replacementPolicy.push_back(std::make_unique<SRRIP>());
    }

    void initialize(CACHE* cache_block) {
        for (auto policy : replacementPolicy) {
            policy->initialize(cache_block);
        }
    }

    void updatePolicyState(CACHE* cache_block, std::uint64_t current_cycle, std::uint32_t triggering_cpu,
                     std::uint32_t set, std::uint32_t way, std::uint64_t full_addr, std::uint64_t ip,
                     std::uint64_t victim_addr, std::uint32_t type, std::uint8_t hit)
        if (current_cycle >= nextUpdateCycle) {
            // update all the MAB stuff
            bandit.updateRewards(currentPolicy, ::current_ipc);
            currentPolicy = bandit.nextArm();

            nextUpdateCycle += MAB_IPC_UPDATE_FREQUENCY;
        }

        updatePolicyState(cache_block, current_cycle, triggering_cpu, instr_id, set, current_set, ip, full_addr, type);
    }

    std::uint32_t findVictim(CACHE* cache_block, std::uint64_t current_cycle, std::uint32_t triggering_cpu,
                             std::uint64_t instr_id, std::uint32_t set, const CACHE::BLOCK* current_set,
                             std::uint64_t ip, std::uint64_t full_addr, std::uint32_t type) override {
        replacementPolicy[currentPolicy]->findVictim(cache_block, current_cycle, triggering_cpu, instr_id, set, current_set, ip, full_addr, type);
    }

   private:
    // updates the internal state of all the policies
    void updatePolicyState(CACHE* cache_block, std::uint64_t current_cycle, std::uint32_t triggering_cpu,
                     std::uint32_t set, std::uint32_t way, std::uint64_t full_addr, std::uint64_t ip,
                     std::uint64_t victim_addr, std::uint32_t type, std::uint8_t hit)
        for (auto policy : replacementPolicy) {
            policy->updateState(cache_block, current_cycle, triggering_cpu, set, way, full_addr, ip, victim_addr, type, hit);
        }
    }

    std::size_t N;
    double c;
    double gamma;
    MultiArmedBandit bandit;
    std::vector<std::unique_ptr<ReplacementPolicy>> replacementPolicy;
    std::size_t currentPolicy;
    // the next cycle during which the MAB is to be updated
    std::uint64_t nextUpdateCycle;

    static constexpr std::uint64_t MAB_IPC_UPDATE_FREQUENCY = 150;
};
