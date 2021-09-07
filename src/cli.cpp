#include <iostream>
#include <string>
#include <unistd.h>

#include "eval/pattern_eval.h"
#include "game/cpu.h"


#define BLACK false
#define WHITE true


void usage(char *argv[]) {
    cerr << "Usage: " << argv[0] << " [-h] [-d DEPTH] [-t TIME] [-e EG_DEPTH] [-w WEIGHTS]" << endl << endl;
    cerr << "\t-h: print this message" << endl;
    cerr << "\t-d DEPTH: search to a maximum depth of DEPTH in midgame (int).\t\tDefault: 30" << endl;
    cerr << "\t-t TIME: spend a maximum of TIME seconds on midgame search (float).\tDefault: 10.0" << endl;
    cerr << "\t-e EG_DEPTH: start using endgame solver at depth EG_DEPTH (int).\tDefault: 23" << endl;
    cerr << "\t-w WEIGHTS: load weights from the file at WEIGHTS (str).\t\tDefault: 'weights.txt'" << endl;
}


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
    // Defaults
    int max_depth = 30;
    float max_time = 10.0;
    int eg_depth = 23;
    string weights_file;

    // Parse command line args to set max_depth, max_time, and/or eg_depth
    char optchar;
    while ((optchar = getopt(argc, argv, "hd:t:e:w:")) != -1) {
        switch (optchar) {
            case 'h':
                usage(argv);
                exit(0);
            case 'd':
                cerr << "Setting depth to " << optarg << endl;
                max_depth = std::stoi(optarg);
                break;
            case 't':
                cerr << "Setting max time per move to " << optarg << endl;
                max_time = std::stof(optarg);
                break;
            case 'e':
                cerr << "Setting endgame depth to " << optarg << endl;
                eg_depth = std::stoi(optarg);
                break;
            case 'w':
                cerr << "Using weights file " << optarg << endl;
                weights_file = optarg;
                break;
            default:
                usage(argv);
                exit(1);
        }
    }

    if (weights_file.empty()) {
        weights_file = "weights.txt";
        cerr << "No weights file specified, using " << weights_file << endl;
    }

    board::Board board = board::starting_position();
    eval::load_weights(weights_file);
    CPU cpu{max_depth, max_time, eg_depth};
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
