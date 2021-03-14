#include "board/board.h"

#include <iostream>


int perft(board::Board b, int depth, bool passed) {
    uint64_t move_mask = board::get_moves(b);

    if (depth == 0) return 1;

    if (move_mask == 0ULL) {
        if (passed) return 1;
        return perft(board::Board{b.opp, b.own}, depth - 1, true);
    }

    int nodes = 0;
    while (move_mask != 0ULL) {
        int m = __builtin_ctzll(move_mask);
        move_mask &= move_mask - 1;

        nodes += perft(board::do_move(b, m), depth - 1, false);
    }

    return nodes;
}


int main() {
    board::Board b;
    b = board::add_piece(b, 27, WHITE);
    b = board::add_piece(b, 28, BLACK);
    b = board::add_piece(b, 35, BLACK);
    b = board::add_piece(b, 36, WHITE);

    cout << perft(b, 11, false) << "\n";

    return 0;
}
