#pragma once

#include <algorithm>
#include <cstddef>
#include <vector>

#include "util.hpp"

// Interface for all policies
class Policy {
    public:
        virtual std::size_t selectNextArm() = 0;
        virtual void updateState(std::size_t, double) = 0;
};

class EGreedy : public Policy {
    public:
        EGreedy(std::size_t N, double eps) : N{N}, epsilon{eps} {
            rewards.assign(N, 0.);
        }

        virtual std::size_t selectNextArm() override {
            double randomNum = randomDouble();

            if (randomNum <= epsilon) {
                // Explore
                return randomInteger(N);
            } else {
                // Exploit
                // Find max element
                auto result = std::max_element(rewards.begin(), rewards.end());
                // Calculate argmax
                std::size_t arm = std::distance(rewards.begin(), result);

                return arm;
            }
        }

        virtual void updateState(std::size_t arm, double reward) override {
            rewards[arm] += reward;
        }
    private:
        std::size_t N;
        double epsilon;
        // The total reward obtained by arm `i`
        std::vector<double> rewards;
};

class UCB : public Policy {
    public:
    private:
};

class DUCB : public Policy {
    public:
    private:
};
