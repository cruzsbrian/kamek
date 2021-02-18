#include "board/board.h"
#include "game/game.h"
#include "util.h"

#include "eval/simple.h"
#include "search/negamax.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <string>
#include <cmath>
#include <thread>
#include <future>


SimpleEvalParams scale(SimpleEvalParams p, int scalar) {
    for (auto i = 0; i < N_PARAMS_SIMPLE; i++) {
        p.p[i] = (p.p[i] * scalar) / 1000;
    }
    return p;
}

SimpleEvalParams normalize(SimpleEvalParams p) {
    int mag_squared = 0;
    for (auto i = 0; i < N_PARAMS_SIMPLE; i++) {
        mag_squared += (p.p[i] * p.p[i]);
    }
    if (mag_squared == 0) return p;
    return scale(p, 1000 * 1000 / sqrt(mag_squared));
}

SimpleEvalParams add(SimpleEvalParams p1, SimpleEvalParams p2) {
    for (auto i = 0; i < N_PARAMS_SIMPLE; i++) {
        p1.p[i] += p2.p[i];
    }
    return p1;
}

SimpleEvalParams random_params(int mag) {
    SimpleEvalParams p;
    for (auto i = 0; i < N_PARAMS_SIMPLE; i++) {
        p.p[i] = (rand() % 2000) - 1000;
    }
    return scale(normalize(p), mag);
}

string to_str(SimpleEvalParams p) {
    string ret = "(";
    for (auto i = 0; i < N_PARAMS_SIMPLE; i++) {
        ret += to_string(p.p[i]);
        if (i < N_PARAMS_SIMPLE - 1) ret += "; ";
    }
    return ret + ")";
}


void training_game(SimpleEvalParams base_params, SimpleEvalParams test_params, promise<int> *score_prom) {
    SimpleEval base_ev{base_params};
    NegamaxSearch base_bot{&base_ev, 4};

    SimpleEval test_ev{test_params};
    NegamaxSearch test_bot{&test_ev, 4};

    Game game1{&base_bot, &test_bot, false};
    Game game2{&test_bot, &base_bot, false};

    int score = (game2.play() - game1.play()) / 2;
    score_prom->set_value(score);

    cout << "." << flush;
}


int main() {
    srand(time(NULL));

    const int n_iters = 100;
    const int n_test_bots = 50;
    const int tweak_range = 100;

    SimpleEvalParams current_best = {207, -41, 327, 28, 917, -52};

    for (auto i = 0; i < n_iters; i++) {
        cout << "Current best: " << to_str(current_best) << "\n";
        
        SimpleEval ev{current_best};
        NegamaxSearch base_bot{&ev, 4};

        SimpleEvalParams deltas[n_test_bots];
        promise<int> score_proms[n_test_bots];
        future<int> score_futures[n_test_bots];
        int scores[n_test_bots];

        for (auto i = 0; i < n_test_bots / 2; i++) {
            deltas[i*2] = random_params(tweak_range);
            deltas[i*2 + 1] = scale(deltas[i*2], -1000);
        }

        for (auto i = 0; i < n_test_bots; i++) {
            score_futures[i] = score_proms[i].get_future();

            SimpleEvalParams test_params = add(current_best, deltas[i]);
            thread th(training_game, current_best, test_params, &(score_proms[i]));
            th.detach();
        }

        for (auto i = 0; i < n_test_bots; i++) scores[i] = score_futures[i].get();

        int sum = 0;
        for (auto i = 0; i < n_test_bots; i++) sum += scores[i];
        int mean = sum / n_test_bots;

        for (auto i = 0; i < n_test_bots; i++) {
            int centered_score = scores[i] - mean;
            current_best = add(current_best, scale(deltas[i], centered_score * 3));
        }

        current_best = normalize(current_best);

        cout << "\n";
    }

    cout << "\nNew best: " << to_str(current_best) << "\n";

    return 0;
}
