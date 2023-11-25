#pragma once

#include <algorithm>
#include <cstddef>
#include <vector>
#include <cmath>
#include "util.hpp"

// Interface for all policies
class Policy {
    public:
        virtual std::size_t selectNextArm() = 0;
        virtual void updateState(std::size_t, double) = 0;
};

class EGreedy : public Policy {
    public:
        EGreedy(std::size_t N, double eps) : N(N), epsilon(eps), roundRobin(true), totalFrequency(0) {
            rewards.assign(N, 0.);
            frequency.assign(N, 0);
        }

        virtual std::size_t selectNextArm() override {

            //check if round robin phase is running
            if(roundRobin)
            {
                //find next element which has not been tried out
                std::vector<int>::iterator armIt = std::find(frequency.begin(), frequency.end(), 0);
                std::size_t arm = std::distance(frequency.begin(), armIt);

                //check if this is the end of the round robin phase
                if(arm == N-1)
                roundRobin = false;

                //return arm
                return arm;
            }


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
             //update the frequency count of this arm and total frequency
            frequency[arm] += 1;
            totalFrequency += 1;

            rewards[arm] = (rewards[arm]*(frequency[arm] - 1) + reward)/totalFrequency;
        }
    private:
        std::size_t N;
        double epsilon;
        bool roundRobin;
        // The total reward obtained by arm `i`
        std::vector<double> rewards;
        std::vector<int> frequency;
        std::size_t totalFrequency;
};

class UCB : public Policy {
    public:
    UCB(std::size_t N, double c): N(N), c(c), roundRobin(true), totalFrequency(0) {
        rewards.assign(N, 0.0);
        frequency.assign(N, 0);
    }

    virtual std::size_t selectNextArm() override {

        //check if round robin phase is running
        if(roundRobin)
        {
            //find next element which has not been tried out
            std::vector<int>::iterator armIt = std::find(frequency.begin(), frequency.end(), 0);
            std::size_t arm = std::distance(frequency.begin(), armIt);

            //check if this is the end of the round robin phase
            if(arm == N-1)
            roundRobin = false;

            //return arm
            return arm;
        }

        //otherwise find arm potential of each arm
        std::vector<double> armPotential(N);

        for(int i = 0; i < armPotential.size(); i++)
        {
            armPotential[i] = rewards[i] + c*std::sqrt(std::log(totalFrequency)/frequency[i]); // use the UCB formula
        }

        // find the arm with maximum potential
        auto result = std::max_element(armPotential.begin(), armPotential.end());
        std::size_t arm = std::distance(armPotential.begin(), result);

        //return optimal arm
        return arm;           
    }

     virtual void updateState(std::size_t arm, double reward) override {

        //update the frequency count of this arm and total frequency
        frequency[arm] += 1;
        totalFrequency += 1;
            
        rewards[arm] = (rewards[arm]*(frequency[arm] - 1) + reward)/totalFrequency;
     }

    private:
    std::size_t N;
    double c;
    std::vector<double> rewards;
    std::vector<int> frequency;
    std::size_t totalFrequency;
    bool roundRobin;
};

class DUCB : public Policy {
    
    public:
    DUCB(std::size_t N, double c): N(N), c(c), roundRobin(true), totalFrequency(0) {
        rewards.assign(N, 0.0);
        frequency.assign(N, 0);
    }

    virtual std::size_t selectNextArm() override {

        //check if round robin phase is running
        if(roundRobin)
        {
            //find next element which has not been tried out
            std::vector<int>::iterator armIt = std::find(frequency.begin(), frequency.end(), 0);
            std::size_t arm = std::distance(frequency.begin(), armIt);

            //check if this is the end of the round robin phase
            if(arm == N-1)
            roundRobin = false;

            //return arm
            return arm;
        }

        //otherwise find arm potential of each arm
        std::vector<double> armPotential(N);

        for(int i = 0; i < armPotential.size(); i++)
        {
            armPotential[i] = rewards[i] + c*std::sqrt(std::log(totalFrequency)/frequency[i]); // use the UCB formula
        }

        // find the arm with maximum potential
        auto result = std::max_element(armPotential.begin(), armPotential.end());
        std::size_t arm = std::distance(armPotential.begin(), result);

        //return optimal arm
        return arm;           
    }

     virtual void updateState(std::size_t arm, double reward) override {

        //update the frequency count of this arm and total frequency
        if(!roundRobin) // DUCB update
        frequency[arm] = gamma*frequency[arm];

        frequency[arm] += 1;

        if(!roundRobin)
        totalFrequency = gamma*totalFrequency + 1; //DUCB update
        else
        totalFrequency += 1; //normal round robin update
            
        rewards[arm] = (rewards[arm]*(frequency[arm] - 1) + reward)/totalFrequency;
     }


    private:
    std::size_t N;
    double c;
    double gamma;
    std::vector<double> rewards;
    std::vector<int> frequency;
    std::size_t totalFrequency;
    bool roundRobin;
};
