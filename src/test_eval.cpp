#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <climits>

#include "board/board.h"
#include "search/basic.h"
#include "util.h"


const unsigned PROGRESS_DOTS = 50;


void runtests(const string &filename) {
    ifstream pos_file(filename);

    vector<ScoredPosition> positions;
    string pos, turn_str, score_str;
    while (pos_file >> pos >> turn_str >> score_str) {
        //TODO
        // bool turn = (turn_str == "Color.BLACK") ? BLACK : WHITE;
        bool turn = false;

        board::Board b = board::from_str(pos);
        int score = std::stoi(score_str);

        if (turn == WHITE) {
            b = board::Board{b.opp, b.own};
            score = -score;
        }

        positions.push_back({b, score});
    }

    cerr << "Running " << positions.size() << " positions\n";
    cerr << "0%";
    for (unsigned i = 0; i < PROGRESS_DOTS - 6; i++) cerr << "-";
    cerr << "100%\n";

    int last_progress = 0;
    int n_tests = 0;

    long nodes;

    for (auto pos : positions) {
        cout << ab_ff(pos.board, -INT_MAX, INT_MAX, 4, false, &nodes) << " ";
        cout << ab_ff(pos.board, -INT_MAX, INT_MAX, 8, false, &nodes) << "\n";

        n_tests++;
        int p = n_tests * PROGRESS_DOTS / positions.size();
        if (p != last_progress) {
            cerr << "." << std::flush;
            last_progress = p;
        }
    }

    cerr << "\n";
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        cerr << "usage: test_eval depth_1 depth_2 POSITIONS_FILE ...\n";
        exit(1);
    }

    for (int i = 1; i < argc; i++) {
        cerr << argv[i] << "\n";

        runtests(argv[i]);

        cerr << "\n";
    }
}
