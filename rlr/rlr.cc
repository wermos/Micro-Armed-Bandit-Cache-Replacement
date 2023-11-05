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
    int entries;
    int accum;
    int rd;
    
    public:
    SetRD():
    accum(0), rd(2), entries(0)
    {
    }

    void addEntry(int age)
    {

       
        accum += age;
        //std::cout<<"accum:"<<accum<<std::endl;
        entries++;

        if(entries == 32)
        {
            rd = std::max((2* accum)/entries,2);
            //std::cout<<"rd"<<rd<<"accum" <<accum<<std::endl;
            entries = 0;
            accum = 0;
        }
    }

    int returnRD()
    {
         
       return rd;
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
       
        // internalCounter++;
        // if(internalCounter%8 == 0)
        // {
        //     internalCounter = 0;
        //     ageCounter++;
        // }
        ageCounter++;
    }

    int returnPriority(int reuseDistance) // calculates all the priorities and outputs final priority
    {
        ageP = ageCounter < reuseDistance ? High : Low;
        typeHitP = typeHit == 1 ? High: Low;
        typeAccessP = typeAccess == 1? High: Low;

        int priority = 8*ageP + typeHitP + typeAccessP;
        return priority;
    }

    int returnAge() // returns age value which is used for rd/recency calculations etc
    {
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

    void reset(int access) // same object can be used to track the next  line that enters this WAY of the set
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
    // is this called even if the set has empty slots? assuming no
   

    std::vector<std::vector<int>>priorityList;
    // store priority, recency and way number of all slots to determine which way to evict
    for(int i = 0; i < NUM_WAY; i++)
    {
        
        priorityList.push_back({feature_entries[this][set*NUM_WAY + i].returnPriority(rd_entries[this][0].returnRD()), feature_entries[this][set*NUM_WAY + i].returnAge(), i});
        
    }

    sort(priorityList.begin(), priorityList.end());
    // std::cout<<"the access type of evicted line is:"<<feature_entries[this][set*NUM_WAY + priorityList[priorityList.size() - 1][2]].returnTypeAccessP()<<std::endl;
    // std::cout<<"the hit type of evicted line is:"<<feature_entries[this][set*NUM_WAY + priorityList[priorityList.size() - 1][2]].returnTypeHitP()<<std::endl;
    // std::cout<<"priority of evicted: "<<priorityList[priorityList.size() - 1][0]<<std::endl;
    // std::cout<<"way of evicted: "<<priorityList[priorityList.size() - 1][2]<<std::endl;

    return priorityList[0][2]; // return way index of least priority slot


}

void CACHE::update_replacement_state(uint32_t triggering_cpu, uint32_t set, uint32_t way, uint64_t full_addr, uint64_t ip, uint64_t victim_addr, uint32_t type,
                                     uint8_t hit)
{
    // what is up with all the write back bullshit happening in every other policy inside `update_replacement_state`
    if (hit && type == WRITEBACK)
    return;
    
    for(int i = 0; i < NUM_WAY; i++)
    {
        if(i == way)
        continue;

       // if(feature_entries[this][set*NUM_WAY + i].occu == 1) // only increment the age of those entries of the set which are occupied(initially we might have empty slots)
        feature_entries[this][set*NUM_WAY + i].countAge(); // increment age for all other WAYS in this set
    }

   
    
    //update hit status and typeAccess and age
    if(hit)
    {
        
        rd_entries[this][0].addEntry(feature_entries[this][set*NUM_WAY + way].returnAge()); // since its a demand hit, use the age of this cache line to alter re-use distance of the set
        feature_entries[this][set*NUM_WAY + way].setAgeCounter(0); // reset the age
        

        feature_entries[this][set*NUM_WAY + way].setTypeHit(1); // I know we need to set this only once per cache line, but what's the harm?
        if(type == PREFETCH)
        {
            feature_entries[this][set*NUM_WAY + way].setTypeAccess(0); // check and update access type
        }
        else
        {
            feature_entries[this][set*NUM_WAY + way].setTypeAccess(1);
        }
    }
    else
    {
        // a miss is filled in this way, we reset the status
        int access = type == PREFETCH? 1 : 0;
        feature_entries[this][set*NUM_WAY + way].reset(access);
    }
}

void CACHE::replacement_final_stats() 
{

}
