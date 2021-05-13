import argparse
from typing import NamedTuple

from progress.bar import IncrementalBar

import othello


DB_HEADER_BYTES = 16
GAME_BYTES = 68
GAME_HEADER_BYTES = 8

DEPTH_BYTE = 14

PERFECT_SCORE_BYTE = 7


class Position(NamedTuple):
    board: str
    turn: othello.Color

class Game(NamedTuple):
    positions: list[Position]
    black_score: int


def parse_header(header_bytes: bytes):
    pass


def parse_move(move: int):
    row = move // 10 - 1
    col = move % 10 - 1
    return othello.Move(row, col)


def parse_game(game_bytes: bytes):
    assert len(game_bytes) == GAME_BYTES

    # perfect_score = game_bytes[PERFECT_SCORE_BYTE]

    move_bytes = game_bytes[GAME_HEADER_BYTES:]
    moves = list(map(parse_move, move_bytes))

    positions = []

    b = othello.Board()
    color = othello.Color.BLACK
    for m in moves:
        # empties = 64 - b.count_pieces(othello.Color.BLACK) - b.count_pieces(othello.Color.WHITE)

        positions.append(Position(b.to_str(color), color))

        # Passes are not encoded, so we need to check if the player can move
        legal = b.get_moves(color)
        # print(f"Legal moves for {color.value}: {legal}")
        if len(legal) == 0:
            color = color.opp()

        # print(f"{color.value} plays {m.row},{m.col}")

        try:
            b.make_move(m, color)
        except othello.InvalidMove:
            print("Invalid Move", m)
            break

        color = color.opp()

    black_score = b.count_pieces(othello.Color.BLACK) - b.count_pieces(othello.Color.WHITE)

    return Game(positions, black_score)


def parse_db(filename: str):
    with open(filename, "rb") as f:
        db_bytes = f.read()

    data_bytes = db_bytes[DB_HEADER_BYTES:]

    n_games = len(data_bytes) // GAME_BYTES

    # print(f"Parsing {filename}: {n_games} games")
    bar = IncrementalBar(f"Parsing {filename}", max=n_games)

    out_file = open(f"out.txt", "a")

    for i in range(len(data_bytes) // GAME_BYTES):
    # for i in range(2):
        game_bytes = data_bytes[i * GAME_BYTES : (i + 1) * GAME_BYTES]
        g = parse_game(game_bytes)

        for p in g.positions:
            # put score in terms of player to move
            score = g.black_score if p.turn == othello.Color.BLACK else -g.black_score

            out_file.write(f"{p.board} {score}\n")

        bar.next()

    bar.finish()


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("filenames", type=str, nargs='+')

    args = parser.parse_args()

    for f in args.filenames:
        parse_db(f)
