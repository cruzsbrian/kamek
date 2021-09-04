#include "board/board.h"
#include "eval/pattern_eval.h"
#include "game/cpu.h"
#include "util.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <filesystem>


using namespace std;


int main(int argc, char *argv[]) {
    CPU *cpu;
    board::Board pos;
    float total_time = 240.; // Default to 4 minute match.
    float time_spent = 0.;

    cerr << filesystem::current_path() << endl;

    while (cin) {
        // Read command from stdin
        string cmd_str;
        getline(cin, cmd_str);

        // Split into words separated by whitespace
        stringstream ss(cmd_str);
        vector<string> command;
        string cmd_word;
        while (ss >> cmd_word) {
            command.push_back(cmd_word);
        }

        if (command[0] == "init") {
            // Send bot info.
            cout << "botinfo name wonky_kong" << endl;
            cout << "botinfo authors Brian Cruz" << endl;

            // Initialize bot.
            eval::load_weights("weights.txt");
            cpu = new CPU(30, 30.0, 24);
            pos = board::starting_position();

            // Send ready signal.
            cout << "ready" << endl;
        }
        else if (command[0] == "match-time") {
            total_time = (float)stoi(command[1]) / 1000;
        }
        else if (command[0] == "position") {
            pos = board::from_str(command[1]);

            if (command[2] == "white") {
                pos = board::Board{pos.opp, pos.own};
            }
        }
        else if (command[0] == "move") {
            int m = notation_to_move(command[1]);
            pos = board::do_move(pos, m);
        }
        else if (command[0] == "go") {
            SearchResult cpu_move = cpu->next_move(pos, (int)((total_time - time_spent) * 1000));
            time_spent += cpu_move.time_spent;

            string pv_str;
            for (unsigned i = 0; i < cpu_move.pv.size(); i++) {
                pv_str += move_to_notation(cpu_move.pv[i]);
                if (i != cpu_move.pv.size() - 1) pv_str += "-";
            }
            cout << "search "
                 << move_to_notation(cpu_move.best_move) << " ";

            if (cpu_move.depth == DEPTH_100) {
                cout << cpu_move.score << " " << "100% ";
            } else if (cpu_move.depth == DEPTH_100W) {
                cout << cpu_move.score << " " << "100%W ";
            } else {
                cout << win_prob(cpu_move.score) << " " << cpu_move.depth << " ";
            }

            cout << pv_str << endl;
        }
        else if (command[0] == "hint") {
            int max_depth = stoi(command[1]);

            int max_millis = stoi(command[2]);
            float max_time = (float)max_millis / 1000.f;

            CPU hint_cpu{max_depth, max_time, 23};
            SearchResult hint_move = hint_cpu.next_move(pos, -1);

            cout << "search " << move_to_notation(hint_move.best_move) << endl;
        }
        else if (command[0] == "quit") {
            break;
        }
        else {
            cout << "Unknown command" << endl;
        }
    }


    delete cpu;

    return 0;
}
