/* CS 2200 - Project 4 - Spring 2016
 * Name - Mingjun Xie
 * GTID - 903074907
 */

#include "cachesim.h"
/* The cache block struct*/
typedef struct cache_block {
    char valid;
    uint64_t tag;
    int dirty;
    uint64_t freq;
 } block;
/* The set struct*/
typedef struct set {
    block* block_entries;
 }set;

/* The cache struct*/
typedef struct cache {
    uint64_t c;
    uint64_t s;
    uint64_t b;
    set* sets;
}cache;
/*The global variable*/
cache* cache_in_use;
/* helper functions */
uint64_t get_offset(uint64_t address);
uint64_t get_index(uint64_t address);
uint64_t get_tag(uint64_t address);
void miss(char rw, uint64_t address, struct cache_stats_t *stats);
uint64_t replace(char rw, uint64_t address, struct cache_stats_t *stats);
/**
 * Sub-routine for initializing your cache with the parameters.
 * You may initialize any global variables here.
 *
 * @param C The total size of your cache is 2^C bytes
 * @param S The set associativity is 2^S
 * @param B The size of your block is 2^B bytes
 */

void cache_init(uint64_t C, uint64_t S, uint64_t B) {
    cache_in_use = (cache*)malloc(sizeof(cache));
    cache_in_use -> c = C;
    cache_in_use -> s = S;
    cache_in_use -> b = B;
    int set_size = 1 << S;
    int cache_lines = 1 << (C-S-B);
    cache_in_use -> sets = (set*)calloc(set_size,(sizeof(set)));
    for (int i = 0; i < set_size; i++) {
        cache_in_use -> sets[i].block_entries = (block*)calloc(cache_lines,(sizeof(block)));
    }
}

/**
 * Subroutine that simulates one cache event at a time.
 * @param rw The type of access, READ or WRITE
 * @param address The address that is being accessed
 * @param stats The struct that you are supposed to store the stats in
 */
void cache_access (char rw, uint64_t address, struct cache_stats_t *stats) {
    uint64_t tag = get_tag(address);
    uint64_t index = get_index(address);
    block b;
    int hit = 0;
    for (int i = 0; i < 1 << cache_in_use -> s; i++) {
        b = cache_in_use -> sets[i].block_entries[index];
        if (b.tag == tag && b.valid) {
            hit = 1;
            cache_in_use -> sets[i].block_entries[index].freq++;
            if (rw == 'w') {
                cache_in_use -> sets[i].block_entries[index].dirty = 1;
            }
        }
    }

    stats -> accesses++;
    if (rw == 'w') {
        stats -> writes++;
    } else {
        stats-> reads++;
    }
    if (!hit) {
        miss(rw,address, stats);
    }
}

/**
 * Subroutine for cleaning up memory operations and doing any calculations
 * Make sure to free malloced memory here.
 *
 */
void cache_cleanup (struct cache_stats_t *stats) {

    int i;
    for (i = 0; i < 1 << cache_in_use -> s; i++) {
        free(cache_in_use -> sets[i].block_entries);
    }
    free(cache_in_use -> sets);
    free(cache_in_use);
    stats->miss_rate = stats->misses /(1.0*stats->accesses);
    stats->avg_access_time = 2 + (stats->miss_rate) * 100; 
}

/**
 * The helper functions
 */

void miss(char rw, uint64_t address, struct cache_stats_t *stats) {
    uint64_t tag = get_tag(address);
    uint64_t index = get_index(address);
    uint64_t victim = 0;
    int hasEmptySpot = 0;
    int i = 0;
    block b;
    for (i = 0; i < 1 << cache_in_use -> s; i++) {
        b = cache_in_use -> sets[i].block_entries[index];
        if (!b.valid && !hasEmptySpot) {
            victim = i;
            hasEmptySpot = 1;
        }
    }

    if (!hasEmptySpot) {
        victim = replace(rw, index, stats);
    }

    if (cache_in_use->sets[victim].block_entries[index].dirty) {
        stats -> write_backs++;
    }

    cache_in_use -> sets[victim].block_entries[index].tag = tag;
    cache_in_use -> sets[victim].block_entries[index].valid = 1;
    cache_in_use -> sets[victim].block_entries[index].freq = 1;

    if (rw == 'w') {
        stats -> write_misses++;
        cache_in_use -> sets[victim].block_entries[index].dirty = 1;
    } else {
        stats -> read_misses++;
        cache_in_use -> sets[victim].block_entries[index].dirty = 0;
    }
    stats -> misses++;
}

uint64_t replace(char rw, uint64_t index, struct cache_stats_t *stats) {
    block temp, vict;
    uint64_t victim = 0;
    for (int i = 0; i < 1 << cache_in_use -> s; i++) {
        temp = cache_in_use -> sets[i].block_entries[index];
        vict = cache_in_use -> sets[victim].block_entries[index];
        if (temp.freq < vict.freq) {
            victim = i;
        }
    }
    return victim;
}
 /**
 * helper function to get the offset bits
 * @param address the address that is being accessed
 */
 uint64_t get_offset(uint64_t address) {
    uint64_t offset_bits = cache_in_use -> b;
    return address & ((1 << offset_bits) - 1 );
 }

 /**
 * helper function to get the index bits
 * @param address the address that is being accessed
 */
 uint64_t get_index(uint64_t address) {
    uint64_t cache_size_bits = cache_in_use -> c;
    uint64_t set_size_bits = cache_in_use -> s;
    uint64_t offset_bits =  cache_in_use -> b;
    uint64_t index_bits = cache_size_bits - set_size_bits - offset_bits;
    return (address >> offset_bits) & ((1 << index_bits) - 1);
 }

 /**
 * helper function to get the tag bits
 * @param address the address that is being accessed
 */
 uint64_t get_tag(uint64_t address) {
    uint64_t cache_size_bits = cache_in_use -> c;
    uint64_t set_size_bits = cache_in_use -> s;
    uint64_t offset_bits =  cache_in_use -> b;
    uint64_t index_bits = cache_size_bits - set_size_bits - offset_bits;
    return address >> (offset_bits + index_bits);
 }