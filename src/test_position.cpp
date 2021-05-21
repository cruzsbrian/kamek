#include <iostream>
#include <string>
#include <climits>

#include "board/board.h"
#include "search/hashtable.h"
#include "search/alphabeta.h"
#include "search/pvs.h"
#include "game/cpu.h"
#include "util.h"


int main(int argc, char *argv[]) {
    if (argc != 4) {
        cerr << "usage: test_position MIN_DEPTH MAX_DEPTH POSITION_STR\n";
        exit(1);
    }

    HashTable ht;

    board::Board b = board::from_str(argv[3]);

    int min_depth = std::stoi(argv[1]);
    int max_depth = std::stoi(argv[2]);

    cout << board::to_grid(b, PIECE_OWN);

    CPU cpu{0, 0};

    cpu.next_move(b, 0);

    /* int n_scores = 0; */
    /* int score_sum = 0; */

    /* for (int i = min_depth; i <= max_depth; i++) { */
    /*     CPU cpu{i, 0}; */
    /*     long nodes; */

    /*     int score = pvs(b, -INT_MAX, INT_MAX, i, ht, false, &nodes).score; */
    /*     cout << "depth " << i << ": " << win_prob(score) << "\n"; */

    /*     score_sum += score; */
    /*     n_scores++; */
    /* } */

    /* cout << "average: " << win_prob(score_sum / n_scores) << "\n"; */

    return 0;
}
