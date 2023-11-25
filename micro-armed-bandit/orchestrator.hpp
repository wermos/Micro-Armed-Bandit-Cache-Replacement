#pragma once

#include "bandit/multi_armed_bandit.hpp"
#include "bandit/policy.hpp"

#include "replacement-policy/replacement_policy.hpp"
#include "replacement-policy/drrip.hpp"
#include "replacement-policy/lru.hpp"
#include "replacement-policy/ship.hpp"
#include "replacement-policy/srrip.hpp"

int curr_policy = 1;  // global variable

class Orchestrator {
   public:
    Orchestrator(std::size_t N) : N(N), c(c), gamma(gamma) {
        DUCB* policy = new DUCB(N, c, gamma);
        bandit = MultiArmedBandit(N, policy);

        replacementPolicy.push_back(new DRRIP());
        replacementPolicy.push_back(new SRRIP());
        replacementPolicy.push_back(new SHIP());
        replacementPolicy.push_back(new LRU());
    }

    void initialize_replacement() {
        for (int i = 0; i < replacementPolicy.size(); i++) {
            replacementPolicy[i]->initialize_replacement();
        }
    }

    uint32_t find_victim(uint32_t triggering_cpu, uint64_t instr_id, uint32_t set, const BLOCK* current_set,
                         uint64_t ip, uint64_t full_addr, uint32_t type) {
        replacementPolicy[curr_policy]->find_victum(triggering_cpu, instr_id, set, current_set, ip, full_addr, type);
    }

    void update_replacement_state(uint32_t triggering_cpu, uint32_t set, uint32_t way, uint64_t full_addr, uint64_t ip,
                                  uint64_t victim_addr, uint32_t type, uint8_t hit) {
        for (int i = 0; i < replacementPolicy.size(); i++) {
            replacementPolicy[i]->update_replacement(triggering_cpu, set, way, full_addr, ip, victim_addr, type, hit);
        }
    }

   private:
    std::size_t N;
    double c;
    double gamma;
    MultiArmedBandit bandit;
    std::vector<ReplacementPolicy*> replacementPolicy;
};
