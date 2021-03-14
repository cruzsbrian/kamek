#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <time.h>

#include "board/board.h"
#include "search/endgame.h"


const int N_TESTS = 492;
const bool DISPLAY = false;


template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

board::Board create_board(std::string position) {
    board::Board b;

    for (unsigned i = 0; i < position.length(); i++) {
        auto ch = position[i];
        if (ch == 'X') b = board::add_piece(b, i, BLACK);
        else if (ch == 'O') b = board::add_piece(b, i, WHITE);
    }

    return b;
}


void runtests(const string &filename) {
    ifstream ffo_file(filename);

    int incorrect = 0;

    float max_time = 0;
    float min_time = 99999;
    float total_time = 0;
    long total_nodes = 0;

    int last_progress = 0;

    for (auto i = 0; i < N_TESTS; i++) {
        string pos, turn_str, move_sol_str, score_sol_str;
        ffo_file >> pos >> turn_str >> move_sol_str >> score_sol_str;

        bool turn = (turn_str == "Black") ? BLACK : WHITE;
        board::Board b = create_board(pos);
        int score_sol = std::stoi(score_sol_str);

        if (turn == WHITE) {
            b = board::Board{b.opp, b.own};
            score_sol = -score_sol;
        }

        if (DISPLAY) cerr << pos << " " << turn_str << "\n";

        endgame::EndgameStats stats;
        int score = endgame::solve(b, stats, DISPLAY);

        total_time += stats.time_spent;
        total_nodes += stats.nodes;
        if (stats.time_spent > max_time) max_time = stats.time_spent;
        if (stats.time_spent < min_time) min_time = stats.time_spent;

        cout << stats.time_spent << "\n";

        if (sgn(score) != sgn(score_sol)) {
            cerr << pos << " " << turn_str << "\n";
            cerr << "Incorrect result: score " << score << ", solution " << score_sol << "\n";

            incorrect++;
        }

        if (DISPLAY) cerr << "\n";
        else {
            int p = i * 50 / N_TESTS;
            if (p != last_progress) {
                cerr << "." << std::flush;
                last_progress = p;
            }
        }
    }

    cerr << "\n";

    float nps = (float)total_nodes / total_time;
    cerr << (float)total_nodes << " nodes in " << total_time << "s @ " << nps << " node/s" << "\n";

    float avg_time = total_time / (float)N_TESTS;
    cerr << "Mean time: " << avg_time << "s\n";
    cerr << "Max time: " << max_time << "s\n";
    /* cerr << "Min time: " << min_time << "s\n"; */

    cerr << "Total incorrect: " << incorrect << "\n";
}


int main() {
    cout << "13-14 empties\n";
    cerr << "Starting depth 13-14\n";
    runtests("ffotest/eg_13_14.txt");

    cout << "\n15-16 empties\n";
    cerr << "\nStarting depth 15-16\n";
    runtests("ffotest/eg_15_16.txt");

    cout << "\n17-18 empties\n";
    cerr << "\nStarting depth 17-18\n";
    runtests("ffotest/eg_17_18.txt");

    cout << "\n19-20 empties\n";
    cerr << "\nStarting depth 19-20\n";
    runtests("ffotest/eg_19_20.txt");

    cout << "\n21-22 empties\n";
    cerr << "\nStarting depth 21-22\n";
    runtests("ffotest/eg_21_22.txt");
}
