#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <climits>

#include "board/board.h"
#include "search/alphabeta.h"
#include "util.h"


const unsigned PROGRESS_DOTS = 50;


void runtests(const string &filename, int depth_1, int depth_2) {
    ifstream pos_file(filename);

    vector<ScoredPosition> positions;
    string pos, turn_str, score_str;
    while (pos_file >> pos >> score_str) {
        board::Board b = board::from_str(pos);
        int score = std::stoi(score_str);

        positions.push_back({b, score});
    }

    cerr << "Running " << positions.size() << " positions\n";
    cerr << "0%";
    for (unsigned i = 0; i < PROGRESS_DOTS - 6; i++) cerr << "-";
    cerr << "100%\n";

    int last_progress = 0;
    int n_tests = 0;

    long nodes;

    HashTable ht;
    for (auto pos : positions) {
        int empties = 64 - board::popcount(pos.board.own | pos.board.opp);

        if (empties > depth_2 && empties < 50) {
            cout << ab_deep(pos.board, -INT_MAX, INT_MAX, depth_1, ht, false, &nodes).score << " ";
            cout << ab_deep(pos.board, -INT_MAX, INT_MAX, depth_2, ht, false, &nodes).score << "\n";
        }

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
    if (argc < 4) {
        cerr << "usage: " << argv[0] << " depth_1 depth_2 POSITIONS_FILE ...\n";
        exit(1);
    }

    int depth_1 = std::stoi(argv[1]);
    int depth_2 = std::stoi(argv[2]);

    for (int i = 3; i < argc; i++) {
        cerr << argv[i] << "\n";

        runtests(argv[i], depth_1, depth_2);

        cerr << "\n";
    }
}
