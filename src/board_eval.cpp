#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>

#include "board.h"
#include "pattern_eval.h"
#include "common.h"


const unsigned PROGRESS_DOTS = 50;



vector<ScoredPosition> parse_file(const string &filename) {
    ifstream pos_file(filename);
    vector<ScoredPosition> positions;

    cerr << "Parsing file..." << "\n";

    string pos, turn_str, score_str;
    while (pos_file >> pos >> score_str) {
        board::Board b = board::from_str(pos);
        int score = std::stoi(score_str);

        positions.push_back({b, score});
    }

    return positions;
}


void eval_boards(string filename) {
    auto positions = parse_file(filename);

    cerr << "Evaluating " << positions.size() << " positions\n";

    ProgressBar prog{positions.size()};
    prog.start();

    for (auto pos : positions) {
        // Get all vals, then rotate 180 deg to get all vals in opposite direction
        int vals1[N_ALL_MASKS];
        int vals2[N_ALL_MASKS];
        eval::pattern_activations(vals1, pos.board);

        board::Board reversed = {eval::flip_horiz(eval::flip_vert(pos.board.own)),
                                 eval::flip_horiz(eval::flip_vert(pos.board.opp))};
        eval::pattern_activations(vals2, reversed);

        /* cout << board::to_grid(pos.board, PIECE_OWN); */
        /* cout << board::to_grid(reversed, PIECE_OWN); */

        int empties = 64 - board::popcount(pos.board.own | pos.board.opp);

        cout << empties << " ";
        for (auto v : vals1) {
            cout << v << " ";
        }
        for (auto v : vals2) {
            cout << v << " ";
        }
        cout << pos.score << "\n";

        prog.step();
    }

    cerr << "\n";
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "usage: board_eval POSITIONS_FILE\n";
        exit(1);
    }

    eval_boards(argv[1]);

    return 0;
}
