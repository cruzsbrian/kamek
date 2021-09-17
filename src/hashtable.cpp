#include "hashtable.h"

#include <cstdlib>
#include <fmt/core.h>


HashTable::HashTable() {
    slots = new TableNode[N_SLOTS];

    zobrist_table = new uint32_t*[16];
    srand(1337);
    for (int i = 0; i < 16; i++) {
        zobrist_table[i] = new uint32_t[256];
        for (int j = 0; j < 256; j++) {
            zobrist_table[i][j] = rand() & MAX_HASH;
        }
    }
}

HashTable::~HashTable() {
    delete[] slots;
}


SearchNode *HashTable::get(board::Board key) {
    TableNode *node = &(slots[hash(key)]);

    if (node->key == key) {
        return &(node->val);
    } else {
        return nullptr;
    }
}


void HashTable::set(board::Board key, SearchNode val) {
    slots[hash(key)] = {key, val};
}


size_t HashTable::hash(board::Board b) {
    size_t ret = 0;

    // Loop through each byte of the board, XOR hash vals together
    const uint8_t *board_bytes = (const uint8_t *) &b;
    for (int i = 0; i < 16; i++) {
        ret ^= zobrist_table[i][board_bytes[i]];
    }

    return ret;
}
