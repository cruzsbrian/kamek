#include "board/board.h"
#include "game/game.h"
#include "util.h"

#include "eval/simple.h"
#include "search/negamax.h"
#include "game/human_player.h"


int main() {
    SimpleEval *ev1 = new SimpleEval({207, -41, 327, 28, 917, -52});
    NegamaxSearch *cpu1 = new NegamaxSearch(ev1, 8);

    SimpleEval *ev2 = new SimpleEval({156, -87, 56, 36, 980, 2});
    NegamaxSearch *cpu2 = new NegamaxSearch(ev2, 8);

    HumanPlayer *human = new HumanPlayer();

    Game game{cpu2, human};
    game.play(true);

    return 0;
}
