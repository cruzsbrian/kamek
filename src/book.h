#pragma once

#include <vector>
#include <queue>
#include <string>

#include "board.h"


namespace book {

struct BookEntry {
    board::Board b;
    int best_move;
};

void load_book(const string &filename);

int search(board::Board b);
    
void build_book(queue<board::Board> &pos_queue, double time_limit, int depth);
void write_book(const string &filename);

} // namespace book
