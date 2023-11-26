#pragma once

#include <cstddef>

#include "policy.hpp"

class MultiArmedBandit {
    public:
        MultiArmedBandit() = default;
        
        MultiArmedBandit(std::size_t numArms, Policy* pol) : N{numArms}, policy{pol} {}

        std::size_t nextArm() {
            return policy->selectNextArm();
        }

        void updateRewards(std::size_t arm, double reward) {
            policy->updateState(arm, reward);
        }
    private:
        // The number of arms
        std::size_t N;
        Policy* policy;
};