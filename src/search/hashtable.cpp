#include "hashtable.h"


TableNode *HashTable::get(board::Board key) {
    TableNode *node = &(slots[hash(key)]);

    if (node->key == key) {
        return node;
    } else {
        return nullptr;
    }
}


void HashTable::set(board::Board key, SearchNode val) {
    slots[hash(key)] = {key, val};
}


size_t HashTable::hash(board::Board b) {
    size_t own_hash = hash_obj(b.own);
    size_t opp_hash = hash_obj(b.opp);

    return own_hash ^ (opp_hash << 1);
}
