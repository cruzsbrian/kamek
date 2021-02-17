#pragma once

#include <string>
#include <cstdint>

using namespace std;


#define BLACK false
#define WHITE true


namespace board {

struct Board {
    uint64_t b = 0L;
    uint64_t w = 0L;
    uint16_t hash = 0;
};


void init_hash();

uint64_t get_moves(Board b, bool c);
int get_frontier(Board b, bool c);
int get_stable(Board b, bool c);

Board do_move(Board b, int pos, bool c);
Board add_piece(Board b, int pos, bool c);

string to_str(Board b);
string to_str(uint64_t mask);

int popcount(uint64_t x);

}
