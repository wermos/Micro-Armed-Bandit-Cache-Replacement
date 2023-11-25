#include "orchestrator.hpp"

#include "cache.h"

void CACHE::initialize_replacement() {

}

uint32_t CACHE::find_victim(uint32_t triggering_cpu, uint64_t instr_id, uint32_t set, const BLOCK* current_set,
                            uint64_t ip, uint64_t full_addr, uint32_t type) {

}

void CACHE::update_replacement_state(uint32_t triggering_cpu, uint32_t set, uint32_t way, uint64_t full_addr,
                                     uint64_t ip, uint64_t victim_addr, uint32_t type, uint8_t hit) {

}

void CACHE::replacement_final_stats() {}
