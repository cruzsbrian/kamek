#include <iostream>
#include <string>
#include <getopt.h>

#include "pattern_eval.h"
#include "cpu.h"


#define BLACK false
#define WHITE true


struct Options {
    int max_depth;
    float max_time;
    int eg_depth;
    string weights_file;
    int cs2;
};

const Options default_opts = {30, 15.0, 24, "weights.txt", 0};


void usage(char *argv[]) {
    cerr << "Usage: " << argv[0] << " [-h] [--cs2] [-d DEPTH] [-t TIME] [-e EG_DEPTH] [-w WEIGHTS]" << endl << endl;
    cerr << "\t-h, --help: print this message" << endl << endl;
    cerr << "\t--cs2: play using the CS2 protocol" << endl << endl;
    cerr << "\t-d DEPTH: search to a maximum depth of DEPTH in midgame (int).\t\t"
         << "Default: " << default_opts.max_depth << endl;
    cerr << "\t-t TIME: spend a maximum of TIME seconds on midgame search (float).\t"
         << "Default: " << default_opts.max_time << endl;
    cerr << "\t-e EG_DEPTH: start using endgame solver at depth EG_DEPTH (int).\t"
         << "Default: " << default_opts.eg_depth << endl;
    cerr << "\t-w WEIGHTS: load weights from the file at WEIGHTS (str).\t\t"
         << "Default: " << default_opts.weights_file << endl;
}


Options parse_opts(int argc, char *argv[]) {
    Options ret = default_opts;

    // Array of long-opt structs for getopt.
    // format: {name, has_arg, *flag, val}
    static struct option long_opts[] = {
        {"cs2", no_argument, &ret.cs2, 1},
        {"help", no_argument, NULL, 'h'}
    };

    // Use GNU getopt to parse args.
    int optchar;
    int optidx = 0;
    while ((optchar = getopt_long(argc, argv, "hd:t:e:w:", long_opts, &optidx)) != -1) {
        switch (optchar) {
            case 0:
                // Case for long_opts. getopt_long will already set the flag, so do nothing.
                break;
            case 'h':
                usage(argv);
                exit(0);
                break;
            case 'd':
                cerr << "Setting depth to " << optarg << endl;
                ret.max_depth = std::stoi(optarg);
                break;
            case 't':
                cerr << "Setting max time per move to " << optarg << endl;
                ret.max_time = std::stof(optarg);
                break;
            case 'e':
                cerr << "Setting endgame depth to " << optarg << endl;
                ret.eg_depth = std::stoi(optarg);
                break;
            case 'w':
                cerr << "Using weights file " << optarg << endl;
                ret.weights_file = optarg;
                break;
            default:
                usage(argv);
                exit(1);
        }
    }

    return ret;
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

void cli_play(Options opts) {
    board::Board board = board::starting_position();
    eval::load_weights(opts.weights_file);
    CPU cpu{opts.max_depth, opts.max_time, opts.eg_depth, true};
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
        } else if (command == "go") { // Go command.
            // Get CPU move
            int move = cpu.next_move(board, -1).node.best_move;
            board = board::do_move(board, move);
            history.push_back(board);
            turn = !turn;

            cout << board::to_grid_moves(board, turn);
            if (game_over(board, turn)) break;

            cout << (turn ? "White" : "Black") << " to move\n";
        } else if (command == "q" || command == "quit") { // Quit command.
            break;
        } else if (!is_notation_valid(command)) { // Check if valid notation.
            cout << "'" << command << "' is not a valid command or move\n";
        } else { // If valid notation, make move.
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
}




void cs2_play(Options opts, bool bot_color) {
    cerr << "Using CS2 mode." << endl;

    board::Board b = board::starting_position();
    eval::load_weights(opts.weights_file);
    CPU cpu{opts.max_depth, opts.max_time, opts.eg_depth, true};

    cout << "Init done.\n";

    int col, row, ms_left;
    bool first_move = true;

    while (cin >> col >> row >> ms_left) {
        // Parse opponent move
        int opp_move;
        if (row == -1 && col == -1) opp_move = MOVE_PASS;
        else opp_move = row * 8 + col;

        // If we're playing black, the first move received will be pass, so we need
        // to ignore it.
        if (!(first_move && opp_move == MOVE_PASS)) {
            b = board::do_move(b, opp_move);
        }

        int bot_move = cpu.next_move(b, ms_left).node.best_move;
        b = board::do_move(b, bot_move);

        // Print bot move
        if (bot_move == -1) {
            cout << "-1 -1" << endl;
        } else {
            row = bot_move / 8;
            col = bot_move % 8;
            cout << col << " " << row << endl;
        }
    }
}




int main(int argc, char *argv[]) {
    Options opts = parse_opts(argc, argv);

    if (opts.cs2) {
        // The last arg should specify color.
        string color_arg = argv[argc - 1];

        if (color_arg == "Black" || color_arg == "black" || color_arg == "BLACK") {
            cs2_play(opts, BLACK);
        } else if (color_arg == "White" || color_arg == "white" || color_arg == "WHITE") {
            cs2_play(opts, WHITE);
        } else {
            cerr << "Unrecognized color " << color_arg << endl;
            exit(1);
        }
    } else {
        cli_play(opts);
    }

    return 0;
}
