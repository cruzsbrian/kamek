#include <iostream>
#include <cstring>

#include "eval/pattern_eval.h"
#include "game/cpu.h"


#define BLACK false
#define WHITE true


bool is_notation_valid(string move_str) {
    return (move_str.compare("pass") == 0 ||
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
    if (argc < 2) {
        cerr << "usage: " << argv[0] << " COLOR [DEPTH]\n";
        exit(1);
    }

    bool bot_color;
    if (strcmp(argv[1], "black") == 0 || strcmp(argv[1], "b") == 0) {
        bot_color = BLACK;
    } else if (strcmp(argv[1], "white") == 0 || strcmp(argv[1], "w") == 0) {
        bot_color = WHITE;
    } else {
        cerr << "use [black, white, b, w] to specify color\n";
        exit(1);
    }

    int max_depth = 20;
    if (argc == 3) {
        try {
            max_depth = std::stoi(argv[2]);
            if (max_depth <= 0) {
                cerr << "depth must be at least 1\n";
                exit(1);
            }
        } catch (...) {
            cerr << "invalid depth\n";
            cerr << "usage: " << argv[0] << " COLOR [DEPTH]\n";
            exit(1);
        }
    }


    board::Board board = board::starting_position();
    eval::load_weights("weights.txt");
    CPU cpu{max_depth, 22};
    vector<board::Board> history;

    history.push_back(board);
    cout << board::to_grid_moves(board, BLACK);
    cout << "Black to move\n";

    if (bot_color == BLACK) {
        board = board::do_move(board, cpu.next_move(board, -1));
        history.push_back(board);

        cout << board::to_grid_moves(board, !bot_color);
        cout << (!bot_color ? "White" : "Black") << " to move\n";
    }

    cout << "\nEnter move: ";

    string command;
    while (cin >> command) {
        if (command.compare("undo") == 0) {
            if (history.size() <= 2) {
                cout << "Nothing to undo\n";
            } else {
                history.pop_back();
                history.pop_back();
                board = history.back();

                cout << board::to_grid_moves(board, !bot_color);
                cout << (!bot_color ? "White" : "Black") << " to move\n";
            }
        } else if (!is_notation_valid(command)) {
            cout << "'" << command << "' is not a valid move or 'undo'\n";
        } else {
            int move = notation_to_move(command);

            if (!is_move_legal(board, move)) {
                cout << move_to_notation(move) << " is not a legal move\n";
            } else {
                // Make move and store history
                board = board::do_move(board, move);
                history.push_back(board);

                cout << board::to_grid_moves(board, bot_color);
                if (game_over(board, bot_color)) break;
                cout << (bot_color ? "White" : "Black") << " to move\n";

                // Get CPU move
                move = cpu.next_move(board, 0);
                board = board::do_move(board, move);
                history.push_back(board);

                cout << board::to_grid_moves(board, !bot_color);
                if (game_over(board, !bot_color)) break;
                cout << (!bot_color ? "White" : "Black") << " to move\n";
            }
        }

        cout << "\nEnter move: ";
    }

    cout << "\n";


    return 0;
}
