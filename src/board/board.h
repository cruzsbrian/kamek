#pragma once

#include <string>
#include <cstdint>

using namespace std;


#define BLACK false
#define WHITE true


namespace board {

struct Board {
    uint64_t own = 0L;
    uint64_t opp = 0L;
};


void init_hash();

uint64_t get_moves(Board b);
int get_frontier(Board b);
int get_stable(Board b);

Board do_move(Board b, int pos);
Board add_piece(Board b, int pos, bool c);

string to_str(Board b);
string to_str(uint64_t mask);

int popcount(uint64_t x);

}
