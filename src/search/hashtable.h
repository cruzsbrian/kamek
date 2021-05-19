#pragma once

#include "../board/board.h"
#include "../util.h"


#define N_SLOTS 0x100000
#define MAX_HASH 0xfffff


struct TableNode {
    board::Board key;
    SearchNode val;
};

class HashTable {
public:
    TableNode *get(board::Board key);
    void set(board::Board key, SearchNode val);
private:
    size_t hash(board::Board b);

    std::hash<uint64_t> hash_obj;
    TableNode slots[N_SLOTS];
};
