#include "board/board.h"

#include <iostream>
#include <time.h>


long perft(board::Board b, int depth, bool color, bool passed, bool print) {
    uint64_t move_mask = board::get_moves(b);

    if (depth == 0) {
        if (print) cout << board::to_str(b, color) << "\n";
        return 1;
    }

    if (move_mask == 0ULL) {
        if (passed) return 1;
        return perft(board::Board{b.opp, b.own}, depth - 1, !color, true, print);
    }

    int nodes = 0;
    while (move_mask != 0ULL) {
        int m = __builtin_ctzll(move_mask);
        move_mask &= move_mask - 1;

        nodes += perft(board::do_move(b, m), depth - 1, !color, false, print);
    }

    return nodes;
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "usage: perft DEPTH\n";
        exit(1);
    }

    int depth;
    try {
        depth = std::stoi(argv[1]);
    } catch (const std::exception &) {
        cerr << "Couldn't parse DEPTH as int\n";
        cerr << "usage: perft DEPTH\n";
        exit(1);
    }

    board::Board b;
    b = board::add_piece(b, 27, WHITE);
    b = board::add_piece(b, 28, BLACK);
    b = board::add_piece(b, 35, BLACK);
    b = board::add_piece(b, 36, WHITE);

    cerr << "Counting nodes to depth " << depth << "\n";

    clock_t start = clock();
    
    long nodes = perft(b, depth, BLACK, false, true);

    clock_t end = clock();
    float time_spent = (float)(end - start) / CLOCKS_PER_SEC;
    float nps = (float)nodes / time_spent;
    cerr << nodes << " nodes\n";
    cerr << time_spent << "s @ " << nps << " node/s\n";

    return 0;
}
