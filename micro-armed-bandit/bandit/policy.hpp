#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <numeric>
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
    EGreedy(std::size_t N, double eps) : N{N}, epsilon{eps}, r_avg{1.}, roundRobin{true} {
        assert((epsilon >= 0 && epsilon <= 1) && "Invalid value of epsilon.");

        rewards.assign(N, 0.);
        frequency.assign(N, 0);
    }

    virtual std::size_t selectNextArm() override {
        // check if round robin phase is running
        if (roundRobin) {
            // find next element which has not been tried out
            auto armIt = std::find(frequency.begin(), frequency.end(), 0);
            std::size_t arm = std::distance(frequency.begin(), armIt);
            
            return arm;
        } else {
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
    }

    virtual void updateState(std::size_t arm, double reward) override {
        // update the frequency count of this arm and total frequency
        if (!roundRobin) {
            reward /= r_avg;  // normalize the incoming reward with r_avg
        }

        frequency[arm]++;
        // update the avg reward of this arm
        rewards[arm] = (rewards[arm] * (frequency[arm] - 1) + reward) / frequency[arm];

        if (std::reduce(frequency.begin(), frequency.end()) == N) {
            // check end of round robin phase
            roundRobin = false;  // turn round robin off

            // calculate the avg reward
            r_avg = std::reduce(rewards.begin(), rewards.end()) / N;

            for (int i = 0; i < rewards.size(); i++) {
                // normalize reward across different arms
                rewards[i] /= r_avg;
            }
        }
    }

   private:
    std::size_t N;
    double epsilon;
    double r_avg;

    bool roundRobin;

    // The total reward obtained by arm `i`
    std::vector<double> rewards;
    // The number of times arm `i` has been pulled
    std::vector<std::size_t> frequency;
};

class UCB : public Policy {
   public:
    UCB(std::size_t N, double c) : N{N}, c{c}, r_avg{1.}, totalFrequency{0}, roundRobin{true} {
        rewards.assign(N, 0.);
        frequency.assign(N, 0);
    }

    virtual std::size_t selectNextArm() override {
        // check if round robin phase is running
        if (roundRobin) {
            // find next element which has not been tried out
            auto armIt = std::find(frequency.begin(), frequency.end(), 0);
            std::size_t arm = std::distance(frequency.begin(), armIt);

            return arm;
        }

        // otherwise find arm potential of each arm
        std::vector<double> armPotential(N);

        for (int i = 0; i < armPotential.size(); i++) {
            armPotential[i] =
                rewards[i] + c * std::sqrt(std::log(totalFrequency) / frequency[i]);  // use the UCB formula
        }

        // find the arm with maximum potential
        auto result = std::max_element(armPotential.begin(), armPotential.end());
        std::size_t arm = std::distance(armPotential.begin(), result);

        // return optimal arm
        return arm;
    }

    virtual void updateState(std::size_t arm, double reward) override {
        // update the frequency count of this arm and total frequency
        if (!roundRobin) {
            reward /= r_avg;  // normalize the incoming reward with r_avg
        }

        frequency[arm]++;
        totalFrequency++;
        // update the avg reward of this arm
        rewards[arm] = (rewards[arm] * (frequency[arm] - 1) + reward) / frequency[arm];

        if (totalFrequency == N) {
            // check end of round robin phase
            roundRobin = false;

            // calculate the avg reward
            r_avg = std::reduce(rewards.begin(), rewards.end()) / N;

            for (int i = 0; i < rewards.size(); i++) {
                rewards[i] /= r_avg;  // normalize reward across different arms
            }
        }
    }

   private:
    std::size_t N;
    double c;
    double r_avg;
    std::size_t totalFrequency;

    bool roundRobin;

    std::vector<double> rewards;
    std::vector<std::size_t> frequency;
};

class DUCB : public Policy {
   public:
    DUCB(std::size_t N, double c, double gamma)
        : N{N}, c{c}, gamma{gamma}, r_avg{1.}, totalFrequency{0}, roundRobin{true} {
        rewards.assign(N, 0.);
        frequency.assign(N, 0);
    }

    virtual std::size_t selectNextArm() override {
        // check if round robin phase is running
        if (roundRobin) {
            // find next element which has not been tried out
            auto armIt = std::find(frequency.begin(), frequency.end(), 0);
            std::size_t arm = std::distance(frequency.begin(), armIt);

            return arm;
        }

        // otherwise find arm potential of each arm
        std::vector<double> armPotential(N);

        for (int i = 0; i < armPotential.size(); i++) {
            armPotential[i] =
                rewards[i] + c * std::sqrt(std::log(totalFrequency) / frequency[i]);  // use the UCB formula
        }

        // find the arm with maximum potential
        auto result = std::max_element(armPotential.begin(), armPotential.end());
        std::size_t arm = std::distance(armPotential.begin(), result);

        // return optimal arm
        return arm;
    }

    virtual void updateState(std::size_t arm, double reward) override {
        // update the frequency count of this arm and total frequency
        if (!roundRobin) {
            // DUCB update
            frequency[arm] *= gamma;
            frequency[arm]++;
            totalFrequency = gamma * totalFrequency + 1;

            reward /= r_avg;  // normalize the incoming reward with r_avg
        } else {
            // normal generic update
            frequency[arm]++;
            totalFrequency++;
        }

        // update the avg reward of this arm
        rewards[arm] = (rewards[arm] * (frequency[arm] - 1) + reward) / frequency[arm];

        if (totalFrequency == N) {
            // check end of round robin phase
            roundRobin = false;

            // calculate the avg reward
            r_avg = std::reduce(rewards.begin(), rewards.end()) / N;

            for (int i = 0; i < rewards.size(); i++) {
                rewards[i] /= r_avg;  // normalize reward across different arms
            }
        }
    }

   private:
    std::size_t N;
    double c;
    double gamma;
    double r_avg;
    std::size_t totalFrequency;
    
    bool roundRobin;

    std::vector<double> rewards;
    std::vector<std::size_t> frequency;
};
