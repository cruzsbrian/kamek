#pragma once

#include <string>
#include <cstdint>

using namespace std;


#define PIECE_OWN false
#define PIECE_OPP true


namespace board {

struct Board {
    uint64_t own = 0L;
    uint64_t opp = 0L;
};

bool operator==(Board b1, Board b2);

Board starting_position();
Board from_str(std::string position);

uint64_t get_moves(Board b);
int get_frontier(Board b);
void get_stable(Board b, int *n_own, int *n_opp);

Board do_move(Board b, int pos);
Board add_piece(Board b, int pos, bool c);

string to_grid(Board b, bool color);
string to_grid_moves(Board b, bool color);
string to_str(Board b, bool color);
string to_str(Board b);
string to_str(uint64_t mask);

int popcount(uint64_t x);

}
