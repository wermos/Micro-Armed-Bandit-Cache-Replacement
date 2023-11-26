#include "orchestrator.hpp"

#include "cache.h"

Orchestrator Director(4);

void CACHE::initialize_replacement() {
    ::Director.initialize(this);
}


void CACHE::update_replacement_state(uint32_t triggering_cpu, uint32_t set, uint32_t way, uint64_t full_addr,
                                     uint64_t ip, uint64_t victim_addr, uint32_t type, uint8_t hit) {
    ::Director.updateState(this, current_cycle, triggering_cpu, set, way, full_addr, ip, victim_addr, type, hit);
}

uint32_t CACHE::find_victim(uint32_t triggering_cpu, uint64_t instr_id, uint32_t set, const BLOCK* current_set,
                            uint64_t ip, uint64_t full_addr, uint32_t type) {
    return ::Director.findVictim(this, current_cycle, triggering_cpu, instr_id, set, current_set, ip, full_addr, type);
}

void CACHE::replacement_final_stats() {}
