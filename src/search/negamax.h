#include "search.h"
#include "../eval/simple.h"

class NegamaxSearch : public SearchAlg {
public:
    NegamaxSearch(const Eval &e, int depth);

    int next_move(board::Board b, bool c, int ms_left);

private:
    SimpleEval eval;
    int max_depth;

    int negamax_score(board::Board b, bool c, int alpha, int beta, int depth);
};
