#pragma once

#include <algorithm>
#include <cstddef>
#include <limits>
#include <random>
#include <vector>

inline double randomDouble() {
    // Returns a random real in [0, 1].
    static std::uniform_real_distribution<double> distribution(
        0.0, std::nextafter(1.0, std::numeric_limits<double>::infinity()));
    static std::mt19937_64 generator;

    return distribution(generator);
}

inline double randomInteger(std::size_t N) {
    // Returns a random integer in {1, 2, 3, ..., N}.
    static std::uniform_int_distribution<double> distribution(1, N);
    static std::mt19937_64 generator;

    return distribution(generator);
}

class Policy {
    public:
        virtual void updateState(std::size_t, double) = 0;
};

class EGreedy : public Policy {
    public:
        EGreedy(std::size_t N, double eps) : N{N}, epsilon{eps} {
            rewards.assign(N, 0.);
        }

        inline std::size_t selectNextArm() {
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

