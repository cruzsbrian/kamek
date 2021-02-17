#include "../game/player.h"
#include "../eval/eval.h"

class NegamaxSearch : public Player {
public:
    NegamaxSearch(const Eval *e, int depth);

    int next_move(board::Board b, bool c, int ms_left);

private:
    const Eval *eval;
    int max_depth;

    int negamax_score(board::Board b, bool c, int alpha, int beta, int depth);
};
