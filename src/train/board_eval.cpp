#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>

#include "../board/board.h"
#include "../eval/simple_eval.h"
#include "../util.h"


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
        vector<int> vals = eval_activations(pos.board);

        int empties = 64 - board::popcount(pos.board.own | pos.board.opp);

        cout << empties << " ";
        for (auto v : vals) {
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
