#include <algorithm>
#include <cassert>
#include <map>
#include <vector>
#include "cache.h"
#include<climits>

enum Type
{
    High = 1,
    Low = 0

};

class SetRD
{
    private:
    std::vector<int> entries;
    int accum;
    
    public:
    SetRD():
    accum(0)
    {
    }

    void addEntry(int age)
    {
        if(entries.size() <= 32)
        {
            entries.push_back(age);
            accum += age;
        }
        else
        {
            entries.push_back(age);
            accum += age - entries[0];
            entries.erase(entries.begin());
        }

    }

    int returnRD()
    {
        if(entries.size() < 32)
        {
            // how do we initialize RD for a set with less that 32 entries?
            // feel like we should allows all the cache line till then to have high age priority, so return a high RD
            return INT_MAX;
        }
        else
        {
            return accum/entries.size(); // just the int part is fine I think
        }
    }

};

class Status
{
    private:
    int ageCounter; // age of the cache line since the last access
    int typeAccess; // if the last access type is a PREFETCH, typeAcess = 0 
    int typeHit; // if this cache line has had a hit before(1), else 0

    Type ageP; // Age Priority
    Type typeAccessP; //Access Priority
    Type typeHitP; //Hit Priority

    int internalCounter = 0; // temp variable used to increment ageCounter every 8th turn, used in `countAge()` function

    public:
    Status():  // initialized with random states
    ageCounter(0), typeAccess(1), typeHit(0)
    {
    }

    void setTypeHit(int hit) // setter for Hit
    {
        typeHit = hit;
    }

    void setTypeAccess(int access) //setter for access
    {
        typeAccess = access;
    }

    void setAgeCounter(int age)
    {
        ageCounter = age;
    }

    void countAge() // counter of age
    {
        internalCounter++;
        if(internalCounter%8 == 0)
        {
            internalCounter = 0;
            ageCounter++;
        }
    }

    int returnPriority(int reuseDistance) // calculates all the priorities and outputs final priority
    {
        ageP = ageCounter < reuseDistance ? High : Low;
        typeHitP = typeHit == 1 ? High: Low;
        typeAccessP = typeAccess != 1? High: Low;

        int priority = 8*ageP + typeHitP + typeAccessP;
        return priority;
    }

    int returnAge() // returns age value which is used for rd/recency calculations etc
    {
        return ageCounter;
    }

    void reset(int access) // same object can be used to track the next cache line that enters this WAY of the set
    {
        ageCounter = 0;
        typeHit = 0;
        typeAccess = access;
        internalCounter = 0;
        occu = 1;
    }
    int occu = 0;

};

std::map<CACHE*, std::vector<Status>> feature_entries;
std::map<CACHE*, std::vector<SetRD>> rd_entries;

void CACHE::initialize_replacement() 
{ 
    feature_entries[this] = std::vector<Status>(NUM_SET * NUM_WAY);
    rd_entries[this] = std::vector<SetRD>(NUM_SET); // how do I initialize rd_entries for each set?
}


uint32_t CACHE::find_victim(uint32_t triggering_cpu, uint64_t instr_id, uint32_t set, const BLOCK* current_set, uint64_t ip, uint64_t full_addr, uint32_t type)
{
    // is this called even if the set has empty slots? assuming yes
    for(int i = 0; i < NUM_WAY; i++) // if we have an empty slot, use that(don't know if we need to do this)
    {
        if(feature_entries[this][set*NUM_WAY + i].occu == 0)
        {
            return i;
        }
    }

    std::vector<std::vector<int>>priorityList;
    // store priority, recency and way number of all slots to determine which way to evict
    for(int i = 0; i < NUM_WAY; i++)
    {
        if(feature_entries[this][set*NUM_WAY + i].occu !=0)
        {
            priorityList.push_back({feature_entries[this][set*NUM_WAY + i].returnPriority(rd_entries[this][set].returnRD()), feature_entries[this][set*NUM_WAY + i].returnAge(), i});
        }
    }

    sort(priorityList.begin(), priorityList.end());
    return priorityList[0][2]; // return way index of least priority slot


}

void CACHE::update_replacement_state(uint32_t triggering_cpu, uint32_t set, uint32_t way, uint64_t full_addr, uint64_t ip, uint64_t victim_addr, uint32_t type,
                                     uint8_t hit)
{
    // what is up with all the write back bullshit happening in every other policy inside `update_replacement_state`

    // update ageCounters for all the WAYS
    for(int i = 0; i < NUM_WAY; i++)
    {
        if(i == way && hit)
        {
            rd_entries[this][set].addEntry(feature_entries[this][set*NUM_WAY + i].returnAge()); // since its a demand hit, use the age of this cache line to alter re-use distance of the set
            feature_entries[this][set*NUM_WAY + i].setAgeCounter(0); // reset the age
            continue;
        }

        if(feature_entries[this][set*NUM_WAY + i].occu == 1) // only increment the age of those entries of the set which are occupied(initially we might have empty slots)
        feature_entries[this][set*NUM_WAY + i].countAge(); // increment age for all other WAYS in this set
    }

    //update hit status and typeAccess
    if(hit)
    {
        feature_entries[this][set*NUM_WAY + way].setTypeHit(1); // I know we need to set this only once per cache line, but what's the harm?
        if(type == PREFETCH)
        {
            feature_entries[this][set*NUM_WAY + way].setTypeAccess(0); // check and update access type

        }
    }
    else
    {
        // a miss is filled in this way, we reset the status
        int access = type == PREFETCH? 0 : 1;
        feature_entries[this][set*NUM_WAY + way].reset(access);
    }
}

void CACHE::replacement_final_stats() 
{

}
