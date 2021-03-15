#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <time.h>
#include <climits>

#include "board/board.h"
#include "search/endgame.h"
#include "util.h"


const bool DISPLAY = false;
const unsigned PROGRESS_DOTS = 50;


board::Board create_board(std::string position) {
    board::Board b;

    for (unsigned i = 0; i < position.length(); i++) {
        auto ch = position[i];
        if (ch == 'X') b = board::add_piece(b, i, BLACK);
        else if (ch == 'O') b = board::add_piece(b, i, WHITE);
    }

    return b;
}


struct ScoredPosition {
    board::Board board;
    int score;
};


void runtests(const string &filename) {
    ifstream ffo_file(filename);

    // Collect all the positions from the file
    vector<ScoredPosition> positions;
    string pos, turn_str, move_sol_str, score_sol_str;
    while (ffo_file >> pos >> turn_str >> move_sol_str >> score_sol_str) {
        bool turn = (turn_str == "Black") ? BLACK : WHITE;
        board::Board b = create_board(pos);
        int score_sol = std::stoi(score_sol_str);

        if (turn == WHITE) {
            b = board::Board{b.opp, b.own};
            score_sol = -score_sol;
        }

        positions.push_back({b, score_sol});
    }


    cout << "Solving " << positions.size() << " positions\n";

    cout << "0%";
    for (unsigned i = 0; i < PROGRESS_DOTS - 6; i++) cout << "-";
    cout << "100%\n";


    int incorrect = 0;

    float max_time = 0;
    float total_time = 0;
    long total_nodes = 0;

    int last_progress = 0;
    int n_tests = 0;

    for (auto pos : positions) {
        if (DISPLAY) cerr << board::to_str(pos.board) << "\n";

        endgame::EndgameStats stats;
        int score = endgame::solve(pos.board, stats, DISPLAY);

        total_time += stats.time_spent;
        total_nodes += stats.nodes;
        if (stats.time_spent > max_time) max_time = stats.time_spent;

        /* cout << stats.time_spent << "\n"; */

        if (sgn(score) != sgn(pos.score)) {
            /* cerr << board::to_str(pos.board) << " " << turn_str << "\n"; */
            /* cerr << "Incorrect result: score " << score << ", solution " << pos.score << "\n"; */

            incorrect++;
        }

        n_tests++;

        if (DISPLAY) cerr << "\n";
        else {
            int p = n_tests * PROGRESS_DOTS / positions.size();
            if (p != last_progress) {
                cerr << "." << std::flush;
                last_progress = p;
            }
        }
    }

    cerr << "\n";

    float nps = (float)total_nodes / total_time;
    cerr << (float)total_nodes << " nodes in " << total_time << "s @ " << nps << " node/s" << "\n";

    float avg_time = total_time / (float)positions.size();
    cerr << "Mean time: " << avg_time << "s\n";
    cerr << "Max time: " << max_time << "s\n";
    /* cerr << "Min time: " << min_time << "s\n"; */

    cerr << "Total incorrect: " << incorrect << "\n";
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "usage: eg_tests positions_file ..." << "\n";
        exit(1);
    }

    for (int i = 1; i < argc; i++) {
        cout << "Running " << argv[i] << "\n";

        runtests(argv[i]);

        cout << "\n";
    }
}
