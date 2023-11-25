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
        EGreedy(std::size_t N, double eps) : N(N), epsilon(eps), roundRobin(true), totalFrequency(0), r_avg(1.0) {
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
            if(!roundRobin) 
            {
                reward /= r_avg;   // normalize the incoming reward with r_avg
            }

            frequency[arm] += 1;
            totalFrequency += 1;


            // update the avg reward of this arm
            rewards[arm] = (rewards[arm]*(frequency[arm] - 1) + reward)/totalFrequency;

            if(totalFrequency == N) //check end of round robin phase
            {
                roundRobin = false; // turn round robin off
                for(int i = 0; i < rewards.size(); i++)
                {
                    r_avg += rewards[i];
                }

                r_avg /= N; // calculate the avg reward

                for(int i = 0; i < rewards.size(); i++)
                {
                    rewards[i] /= r_avg;  // normalize reward across different arms
                }
                
            }
        }
    private:
        std::size_t N;
        double epsilon;
        bool roundRobin;
        // The total reward obtained by arm `i`
        std::vector<double> rewards;
        std::vector<int> frequency;
        std::size_t totalFrequency;
        double r_avg;
};

class UCB : public Policy {
    public:
    UCB(std::size_t N, double c): N(N), c(c), roundRobin(true), totalFrequency(0), r_avg(1.0) {
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
        if(!roundRobin) 
        {
            reward /= r_avg;   // normalize the incoming reward with r_avg
        }

        frequency[arm] += 1;
        totalFrequency += 1;
        

        // update the avg reward of this arm
        rewards[arm] = (rewards[arm]*(frequency[arm] - 1) + reward)/totalFrequency;

        if(totalFrequency == N) //check end of round robin phase
        {
            roundRobin = false;
            for(int i = 0; i < rewards.size(); i++)
            {
                r_avg += rewards[i];
            }

            r_avg /= N; // calculate the avg reward

            for(int i = 0; i < rewards.size(); i++)
            {
                rewards[i] /= r_avg;  // normalize reward across different arms
            }
            
        }
        
     }

    private:
    std::size_t N;
    double c;
    std::vector<double> rewards;
    std::vector<int> frequency;
    std::size_t totalFrequency;
    bool roundRobin;
    double r_avg;
};

class DUCB : public Policy {
    
    public:
    DUCB(std::size_t N, double c): N(N), c(c), roundRobin(true), totalFrequency(0), r_avg(1.0) {
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
        {
            frequency[arm] = gamma*frequency[arm];
            frequency[arm] += 1;
            totalFrequency = gamma*totalFrequency + 1;
            reward /= r_avg;   // normalize the incoming reward with r_avg
        }
        else // normal generic update
        {
            frequency[arm]  = 1;
            totalFrequency += 1;
        }

        // update the avg reward of this arm
        rewards[arm] = (rewards[arm]*(frequency[arm] - 1) + reward)/totalFrequency;

        if(totalFrequency == N) //check end of round robin phase
        {
            roundRobin = false;
            for(int i = 0; i < rewards.size(); i++)
            {
                r_avg += rewards[i];
            }

            r_avg /= N; // calculate the avg reward

            for(int i = 0; i < rewards.size(); i++)
            {
                rewards[i] /= r_avg;  // normalize reward across different arms
            }
            
        }
     }


    private:
    std::size_t N;
    double c;
    double gamma;
    double r_avg;
    std::vector<double> rewards;
    std::vector<int> frequency;
    std::size_t totalFrequency;
    bool roundRobin;
};
