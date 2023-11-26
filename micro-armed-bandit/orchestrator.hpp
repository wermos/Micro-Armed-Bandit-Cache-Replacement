#pragma once

#include <iostream>
#include <cstdint>
#include <memory>
#include <vector>

#include "cache.h"
#include "ooo_cpu.h"

#include "../bandit/multi_armed_bandit.hpp"
#include "../bandit/policy.hpp"

#include "../replacement-policy/drrip.hpp"
#include "../replacement-policy/lru.hpp"
#include "../replacement-policy/ship.hpp"
#include "../replacement-policy/srrip.hpp"

class Orchestrator {
   public:
    Orchestrator(std::size_t n) : N{n}, c{0.01}, gamma{0.975}, currentPolicy{0}, nextUpdateCycle{0} {
        DUCB* policy = new DUCB(N, c, gamma);
        bandit = MultiArmedBandit(N, policy);

        replacementPolicy.push_back(std::make_shared<LRU>());
        replacementPolicy.push_back(std::make_shared<DRRIP>());
        replacementPolicy.push_back(std::make_shared<SHIP>());
        replacementPolicy.push_back(std::make_shared<SRRIP>());
    }

    void initialize(CACHE* cache_block) {
        for (const auto& policy : replacementPolicy) {
            policy->initialize(cache_block);
        }
    }

    void updateState(CACHE* cache_block, std::uint64_t current_cycle, std::uint32_t triggering_cpu,
                     std::uint32_t set, std::uint32_t way, std::uint64_t full_addr, std::uint64_t ip,
                     std::uint64_t victim_addr, std::uint32_t type, std::uint8_t hit) {
        if (current_cycle >= nextUpdateCycle) {
            // update all the MAB stuff
            bandit.updateRewards(currentPolicy, ::current_ipc);
            currentPolicy = bandit.nextArm();

            nextUpdateCycle += MAB_IPC_UPDATE_FREQUENCY;
        }

        if (currentPolicy == 0)
            std::cout << "Using LRU\n";
        else if (currentPolicy == 1)
            std::cout << "Using DRRIP\n";
        else if (currentPolicy == 2)
            std::cout << "Using SHIP\n";
        else if (currentPolicy == 3)
            std::cout << "Using SRRIP\n";

        updatePolicyStates(cache_block, current_cycle, triggering_cpu, set, way, full_addr, ip, victim_addr, type, hit);
    }

    std::uint32_t findVictim(CACHE* cache_block, std::uint64_t current_cycle, std::uint32_t triggering_cpu,
                             std::uint64_t instr_id, std::uint32_t set, const CACHE::BLOCK* current_set,
                             std::uint64_t ip, std::uint64_t full_addr, std::uint32_t type) {
        return replacementPolicy[currentPolicy]->findVictim(cache_block, current_cycle, triggering_cpu, instr_id, set, current_set, ip, full_addr, type);
    }

   private:
    // updates the internal state of all the policies
    void updatePolicyStates(CACHE* cache_block, std::uint64_t current_cycle, std::uint32_t triggering_cpu,
                     std::uint32_t set, std::uint32_t way, std::uint64_t full_addr, std::uint64_t ip,
                     std::uint64_t victim_addr, std::uint32_t type, std::uint8_t hit) {
        for (const auto& policy : replacementPolicy) {
            policy->updateState(cache_block, current_cycle, triggering_cpu, set, way, full_addr, ip, victim_addr, type, hit);
        }
    }

    std::size_t N;
    double c;
    double gamma;
    MultiArmedBandit bandit;
    std::vector<std::shared_ptr<ReplacementPolicy>> replacementPolicy;
    std::size_t currentPolicy;
    // the next cycle during which the MAB is to be updated
    std::uint64_t nextUpdateCycle;

    static constexpr std::uint64_t MAB_IPC_UPDATE_FREQUENCY = 1000;
};
