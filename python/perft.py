import copy

import othello


def perft(b: othello.Board, c: othello.Color, depth: int, passed: bool):
    if (depth == 0): return 1;

    moves = b.get_moves(c)

    # moves1 = b.old_get_moves(c)
    # if moves != moves1:
    #     print(b.pretty())
    #     print(c.value, moves)
    #     print(c.value, moves1)
    #     print()

    if len(moves) == 0:
        if passed: return 1
        return perft(b, c.opp(), depth - 1, True)

    nodes = 0
    for m in moves:
        b1 = copy.deepcopy(b)
        b1.make_move(m, c)
        nodes += perft(b1, c.opp(), depth - 1, True)

    return nodes


b = othello.Board()
print(perft(b, othello.Color.BLACK, 8, False))
