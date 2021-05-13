#include "game.h"

#include "../board/board.h"
#include "../util.h"

#include <iostream>
#include <string>


using namespace std;


Game::Game(Player *black, Player *white) {
    players[BLACK] = black;
    players[WHITE] = white;

    b = board::add_piece(b, 27, WHITE);
    b = board::add_piece(b, 28, BLACK);
    b = board::add_piece(b, 35, BLACK);
    b = board::add_piece(b, 36, WHITE);
}


Game::Game(board::Board start, Player *black, Player *white) {
    players[BLACK] = black;
    players[WHITE] = white;

    b = start;
}


const string turns[2] = {"BLACK", "WHITE"};

int Game::play(bool display) {
    bool turn = BLACK;
    bool prev_pass = false;

    while (true) {
        if (display)
            cout << board::to_grid(b, turn) << "\n";

        Player *p = players[turn];

        int move = p->next_move(b, -1);
        if (move == -1) {
            if (prev_pass) break;
            else prev_pass = true;
        } else {
            prev_pass = false;
        }

        if (display)
            cout << "\n" << turns[turn] << ": " << move_to_notation(move) << "\n";

        b = board::do_move(b, move);

        turn = !turn;
    }

    int score = board::popcount(b.own) - board::popcount(b.opp);

    if (turn == WHITE) score = -score;

    if (display)
        cout << "Score for black: " << score << "\n";

    return score;
}
