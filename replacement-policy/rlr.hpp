#include <algorithm>
#include <climits>
#include <cstddef>
#include <map>
#include <vector>

#include "cache.h"

#include "replacement_policy.hpp"

enum Type {
    High = 1,
    Low = 0
};

class SetRD {
    private:
    int accum;
    int rd;
    int entries;

    public:
    SetRD() : accum(0), rd(2), entries(0) {}

    void addEntry(int age) {
        accum += age;
        entries++;

        if (entries == 32) {
            rd = std::max((2 * accum) / entries, 2);
            entries = 0;
            accum = 0;
        }
    }

    int returnRD() {
        return rd;
    }
};

class Status {
    private:
    int ageCounter;  // age of the cache line since the last access
    int typeAccess;  // if the last access type is a PREFETCH, typeAcess = 0
    int typeHit;     // if this cache line has had a hit before(1), else 0

    Type ageP;         // Age Priority
    Type typeAccessP;  // Access Priority
    Type typeHitP;     // Hit Priority

    int internalCounter = 0;  // temp variable used to increment ageCounter
                              // every 8th turn, used in `countAge()` function
    int occu = 0;

    public:
    // initialized with random states
    Status() : ageCounter(0), typeAccess(1), typeHit(0) {}

    // setter for Hit
    void setTypeHit(int hit) {
        typeHit = hit;
    }

    // setter for access
    void setTypeAccess(int access) {
        typeAccess = access;
    }

    void setAgeCounter(int age) {
        ageCounter = age;
    }

    // counter of age
    void countAge() {
        ageCounter++;
    }

    // calculates all the priorities and outputs final priority
    int returnPriority(int reuseDistance) {
        ageP = ageCounter < reuseDistance ? High : Low;
        typeHitP = typeHit == 1 ? High: Low;
        typeAccessP = typeAccess == 1? High: Low;

        int priority = 8 * ageP + typeHitP + typeAccessP;
        return priority;
    }

    // returns age value which is used for rd/recency calculations etc
    int returnAge() {
        return ageCounter;
    }

    int returnTypeAccessP()
    {
        return typeAccessP;
    }
    int returnTypeHitP()
    {
        return typeHitP;
    }

    // same object can be used to track the next cache line that enters this WAY of the set
    void reset(int access) {
        ageCounter = 0;
        typeHit = 0;
        typeAccess = access;
        internalCounter = 0;
        occu = 1;
    }
};

class RLR : public ReplacementPolicy {
   public:
    RLR() = default;

    virtual void initialize(CACHE* cache_block) override {
        feature_entries[cache_block] = std::vector<Status>(cache_block->NUM_SET * cache_block->NUM_WAY);
        rd_entries[cache_block] = std::vector<SetRD>(cache_block->NUM_SET);
    }

    virtual void updateState(CACHE* cache_block, std::uint64_t current_cycle, std::uint32_t triggering_cpu,
                             std::uint32_t set, std::uint32_t way, std::uint64_t full_addr, std::uint64_t ip,
                             std::uint64_t victim_addr, std::uint32_t type, std::uint8_t hit) override {
        if (hit && access_type{type} == access_type::WRITE) {
            return;
        }
        
        for(std::uint32_t i = 0; i < cache_block->NUM_WAY; i++) {
            if (i == way) {
                continue;
            }

            feature_entries[cache_block][set*cache_block->NUM_WAY + i].countAge(); // increment age for all other WAYS in this set
        }

        // update hit status and typeAccess and age
        if(hit) {
            rd_entries[cache_block][0].addEntry(feature_entries[cache_block][set*cache_block->NUM_WAY + way].returnAge()); // since its a demand hit, use the age of this cache line to alter re-use distance of the set
            feature_entries[cache_block][set*cache_block->NUM_WAY + way].setAgeCounter(0); // reset the age
            

            feature_entries[cache_block][set*cache_block->NUM_WAY + way].setTypeHit(1); // I know we need to set this only once per cache line, but what's the harm?
            if(access_type{type} == access_type::PREFETCH) {
                feature_entries[cache_block][set*cache_block->NUM_WAY + way].setTypeAccess(0); // check and update access type
            } else {
                feature_entries[cache_block][set*cache_block->NUM_WAY + way].setTypeAccess(1);
            }
        } else {
            // a miss is filled in this way, we reset the status
            int access = access_type{type} == access_type::PREFETCH? 0 : 1;
            feature_entries[cache_block][set*cache_block->NUM_WAY + way].reset(access);
        }
    }

    virtual std::uint32_t findVictim(CACHE* cache_block, std::uint64_t current_cycle, std::uint32_t triggering_cpu,
                                     std::uint64_t instr_id, std::uint32_t set, const CACHE::BLOCK* current_set,
                                     std::uint64_t ip, std::uint64_t full_addr, std::uint32_t type) override {
    std::vector<std::vector<int>>priorityList;
    
    // store priority, recency and way number of all slots to determine which way to evict
    for(int i = 0; i < cache_block->NUM_WAY; i++) {
        priorityList.push_back({feature_entries[cache_block][set*cache_block->NUM_WAY + i].returnPriority(rd_entries[cache_block][0].returnRD()), feature_entries[cache_block][set*cache_block->NUM_WAY + i].returnAge(), i});
    }

    sort(priorityList.begin(), priorityList.end());

    return priorityList[0][2]; // return way index of least priority slot
    }

   private:
    std::map<CACHE*, std::vector<Status>> feature_entries;
    std::map<CACHE*, std::vector<SetRD>> rd_entries;
};
