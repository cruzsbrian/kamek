#include <iostream>
#include <string>

#include "eval/pattern_eval.h"
#include "game/cpu.h"


#define BLACK false
#define WHITE true


bool is_notation_valid(string move_str) {
    return (move_str == "pass" || move_str == "PASS" ||
            (move_str.size() == 2 &&
            'a' <= move_str[0] && move_str[0] <= 'h' &&
            '1' <= move_str[1] && move_str[1] <= '8'));
}

bool is_move_legal(board::Board b, int move) {
    uint64_t move_mask = board::get_moves(b);

    // Passing is legal when no moves are available
    if (move_mask == 0ULL && move == -1) return true;

    bool valid = false;
    while (move_mask != 0ULL) {
        int m = __builtin_ctzll(move_mask);
        move_mask &= move_mask - 1;

        if (m == move) valid = true;
    }

    return valid;
}

bool game_over(board::Board b, bool color) {
    uint64_t move_mask = board::get_moves(b);
    if (move_mask != 0ULL) return false;

    move_mask = board::get_moves(board::Board{b.opp, b.own});
    if (move_mask != 0ULL) return false;

    // Print score
    int own_score = board::popcount(b.own);
    int opp_score = board::popcount(b.opp);
    cout << "Black: " << (color ? opp_score : own_score) << " \t"
         << "White: " << (color ? own_score : opp_score);
    return true;
}


int main(int argc, char *argv[]) {
    int max_depth = 30;
    if (argc == 2) {
        try {
            max_depth = std::stoi(argv[1]);
            if (max_depth <= 0) {
                cerr << "depth must be at least 1\n";
                exit(1);
            }
        } catch (...) {
            cerr << "invalid depth\n";
            cerr << "usage: " << argv[0] << " [DEPTH]\n";
            exit(1);
        }
    }


    board::Board board = board::starting_position();
    eval::load_weights("weights.txt");
    CPU cpu{max_depth, 10.0, 23};
    vector<board::Board> history;
    bool turn = BLACK;

    history.push_back(board);
    cout << board::to_grid_moves(board, BLACK);
    cout << "Black to move\n";
    cout << "\n> ";

    string command;
    while (cin >> command) {
        if (command == "undo") { // Undo command.
            if (history.size() <= 1) {
                cout << "Nothing to undo\n";
            } else {
                history.pop_back();
                board = history.back();
                turn = !turn;

                cout << board::to_grid_moves(board, turn);
                cout << (turn ? "White" : "Black") << " to move\n";
            }
        }
        else if (command == "go") { // Go command.
            // Get CPU move
            int move = cpu.next_move(board, -1).best_move;
            board = board::do_move(board, move);
            history.push_back(board);
            turn = !turn;

            cout << board::to_grid_moves(board, turn);
            if (game_over(board, turn)) break;
            cout << (turn ? "White" : "Black") << " to move\n";
        }
        else if (command == "q" || command == "quit") { // Quit command.
            break;
        }
        else if (!is_notation_valid(command)) { // Check if valid notation.
            cout << "'" << command << "' is not a valid move or 'undo'\n";
        } 
        else { // If valid notation, make move.
            int move = notation_to_move(command);

            if (!is_move_legal(board, move)) {
                cout << move_to_notation(move) << " is not a legal move\n";
            } else {
                // Make move and store history
                board = board::do_move(board, move);
                history.push_back(board);
                turn = !turn;

                cout << board::to_grid_moves(board, turn);
                if (game_over(board, turn)) break;
                cout << (turn ? "White" : "Black") << " to move\n";
            }
        }

        cout << "\n> ";
    }

    cout << "\n";


    return 0;
}
