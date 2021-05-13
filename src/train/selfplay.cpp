#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "../board/board.h"
#include "../game/game.h"
#include "../game/cpu.h"


const unsigned PROGRESS_DOTS = 50;

const int CPU_DEPTH = 4;


vector<board::Board> parse_file(const string &filename) {
    ifstream pos_file(filename);
    vector<board::Board> boards;

    string pos;
    while (pos_file >> pos) {
        boards.push_back(board::from_str(pos));
    }

    return boards;
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        cerr << "usage: self_play POSITIONS_FILE ...\n";
        exit(1);
    }

    vector<board::Board> boards;
    for (int i = 1; i < argc; i++) {
        auto b = parse_file(argv[i]);
        boards.insert(boards.end(), b.begin(), b.end());
    }

    cerr << "Playing " << boards.size() << " positions\n";
    cerr << "0%";
    for (unsigned i = 0; i < PROGRESS_DOTS - 6; i++) cerr << "-";
    cerr << "100%\n";

    int n_tests = 0;
    int last_progress = 0;
    for (auto b : boards) {
        vector<string> board_strs;
        vector<bool> turns;

        CPU players[2] = { CPU{CPU_DEPTH,0}, CPU{CPU_DEPTH,0} };
        bool turn = BLACK;
        bool prev_pass = false;

        // Play the game, recording all boards seen
        int move;
        do {
            board_strs.push_back(board::to_str(b));
            turns.push_back(turn);

            move = players[turn].next_move(b, -1);

            prev_pass = (move == -1);

            b = board::do_move(b, move);
            turn = !turn;
        } while (!(move == -1 && prev_pass));

        // Find score
        int black_score = board::popcount(b.own) - board::popcount(b.opp);
        if (turn == WHITE) black_score = -black_score;

        // Print all the boards with the score
        for (auto i = 0; i < board_strs.size(); i++) {
            int score_for_player = black_score;
            if (turns[i] == WHITE) score_for_player = -black_score;

            cout << board_strs[i] << " " << score_for_player << "\n";
        }

        n_tests++;
        int p = (n_tests * PROGRESS_DOTS) / boards.size();
        if (p != last_progress) {
            last_progress = p;
            cerr << ".";
        }
    }

    cerr << "\n";

    return 0;
}
