from typing import NamedTuple
import argparse

from progress.bar import IncrementalBar

import othello


class Position(NamedTuple):
    board: str
    turn: othello.Color

class Game(NamedTuple):
    positions: list[Position]
    black_score: int


def parse_move(move: str):
    color = othello.Color.BLACK if move[0] == "+" else othello.Color.WHITE
    row = int(move[2]) - 1
    col = ord(move[1]) - ord('a')
    return color, othello.Move(row, col)


def parse_game(game_str: str):
    all_moves = game_str[:-9]
    # print(all_moves)

    move_strs = [all_moves[i:i+3] for i in range(0, len(all_moves), 3)]
    # print(move_strs)

    positions = []

    b = othello.Board()

    for color, move in map(parse_move, move_strs):
        positions.append(Position(b.to_str(color), color))

        b.make_move(move, color)

    black_score = b.count_pieces(othello.Color.BLACK) - b.count_pieces(othello.Color.WHITE)

    return Game(positions, black_score)


def parse_file(filename: str):
    lines = [x for x in open(filename, "r")]

    bar = IncrementalBar(f"Parsing {filename}", max=len(lines))

    out_file = open("out.txt", "a")

    for l in lines:
        g = parse_game(l)

        for p in g.positions:
            # put score in terms of player to move
            score = g.black_score if p.turn == othello.Color.BLACK else -g.black_score

            out_file.write(f"{p.board} {score}\n")

        bar.next()

    bar.finish()


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("filename", type=str)

    args = parser.parse_args()

    parse_file(args.filename)
