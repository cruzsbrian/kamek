#include "human_player.h"

#include "../util.h"

#include <iostream>
#include <string>


int HumanPlayer::next_move(board::Board b, bool c, int ms_left) {
    uint64_t legal_moves = board::get_moves(b, c);

    int move;
    if (legal_moves == 0ULL) {
        cout << "\nMust pass\n";
        move = -1;
    } else {
        string input;
        cout << "\nEnter move: ";
        cin >> input;
        move = notation_to_move(input);
    }

    return move;
}
