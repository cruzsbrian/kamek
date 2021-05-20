#pragma once

#include "../board/board.h"
#include "../util.h"


#define N_SLOTS 0x10000
#define MAX_HASH 0xffff


struct TableNode {
    board::Board key;
    SearchNode val;
};

class HashTable {
public:
    HashTable();
    ~HashTable();
    SearchNode *get(board::Board key);
    void set(board::Board key, SearchNode val);
    size_t hash(board::Board b);
private:
    std::hash<uint64_t> hash_obj;
    TableNode *slots;
    uint32_t **zobrist_table;
};
