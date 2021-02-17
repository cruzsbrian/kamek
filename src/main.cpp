#include "board/board.h"
#include "game/game.h"
#include "util.h"

#include "eval/simple.h"
#include "search/negamax.h"
#include "game/stream_player.h"

#include <iostream>
#include <string>


#define GAME_OVER(move) \
        if (move == -1) { \
            if (prev_pass) break; \
            else prev_pass = true; \
        } else { \
            prev_pass = false; \
        }


using namespace std;


int main() {
    SimpleEval *ev = new SimpleEval();
    NegamaxSearch *cpu1 = new NegamaxSearch(ev, 6);
    NegamaxSearch *cpu2 = new NegamaxSearch(ev, 5);

    StreamPlayer *human = new StreamPlayer();

    Game game{cpu1, cpu2};

    game.play();

    return 0;
}
