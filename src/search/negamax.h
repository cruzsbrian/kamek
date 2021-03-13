#include "../game/player.h"
#include "../eval/eval.h"

class NegamaxSearch : public Player {
public:
    NegamaxSearch(Eval *e, int depth);

    int next_move(board::Board b, int ms_left);

private:
    Eval *eval;
    int max_depth;

    int negamax_score(board::Board b, int alpha, int beta, int depth, bool passed, long *n);
};
