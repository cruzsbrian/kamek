from enum import Enum
from typing import Optional, NamedTuple

import numpy as np


class Color(Enum):
    WHITE = "white"
    BLACK = "black"

    def opp(self):
        return Color.WHITE if self == Color.BLACK else Color.BLACK

    def __repr__(self):
        return self.value


class Move(NamedTuple):
    row: int
    col: int

    def __repr__(self):
        colchar = chr(self.col + ord('a'))
        return colchar + str(self.row)


piece_idx = np.arange(64).reshape(8,8)

def extract_row(x: np.ndarray, row ,col):
    return x[row,:]

def extract_col(x: np.ndarray, row ,col):
    return x[:,col]

def extract_diag(x: np.ndarray, row ,col):
    return x.diagonal(col - row)

def extract_adiag(x: np.ndarray, row ,col):
    return np.fliplr(x).diagonal(7 - col - row)

def get_moves(pieces, piece_idx, color):
    moves = []

    opp_block = False # if we're currently in a block of opponent pieces
    empty_before = None # if we saw an empty square before the block
    mine_before = False # if we saw our piece before the block

    for i in range(len(pieces)):
        if pieces[i] == color * -1:
            opp_block = True
        elif pieces[i] == color:
            if opp_block and empty_before is not None:
                moves.append(empty_before)
            opp_block = False
            empty_before = None
            mine_before = True
        else:
            if opp_block and mine_before:
                moves.append(piece_idx[i])
            opp_block = False
            empty_before = piece_idx[i]
            mine_before = False

    return moves

def get_flips(pieces, piece_idx, move_idx, color):
    flips = []
    opp_after = []
    can_flip_before = False
    can_flip_after = True

    for i in range(len(pieces)):
        if piece_idx[i] < move_idx:
            if pieces[i] == color:
                flips = []
                can_flip_before = True
            if pieces[i] == 0:
                flips = []
                can_flip_before = False
            if pieces[i] == color * -1 and can_flip_before:
                flips.append(piece_idx[i])

        if piece_idx[i] > move_idx:
            if pieces[i] == color:
                flips += opp_after
                break
            if pieces[i] == color * -1 and can_flip_after:
                opp_after.append(piece_idx[i])
            if pieces[i] == 0:
                break

    return flips


class Board:
    pieces: np.ndarray

    def __init__(self):
        self.pieces = np.zeros((8,8))
        self.pieces[3,3] = -1
        self.pieces[3,4] =  1
        self.pieces[4,3] =  1
        self.pieces[4,4] = -1

    def get_flips(self, move: Move, color: Color):
        row       = extract_row(self.pieces, move.row, move.col)
        row_idx   = extract_row(piece_idx, move.row, move.col)
        col       = extract_col(self.pieces, move.row, move.col)
        col_idx   = extract_col(piece_idx, move.row, move.col)
        diag      = extract_diag(self.pieces, move.row, move.col)
        diag_idx  = extract_diag(piece_idx, move.row, move.col)
        adiag     = extract_adiag(self.pieces, move.row, move.col)
        adiag_idx = extract_adiag(piece_idx, move.row, move.col)

        move_idx = move.row * 8 + move.col
        c = 1 if color == Color.BLACK else -1

        flips = []
        flips += get_flips(row, row_idx, move_idx, c)
        flips += get_flips(col, col_idx, move_idx, c)
        flips += get_flips(diag, diag_idx, move_idx, c)
        flips += get_flips(adiag, adiag_idx, move_idx, c)

        return flips

    def make_move(self, move: Move, color: Color):
        if not move or move.row == move.col == -1:
            if len(self.get_moves(color)) != 0:
                raise InvalidMove()
            return

        flips = self.get_flips(move, color)

        if len(flips) == 0:
            print(self.pretty(), color, move)
            raise InvalidMove()

        for i in flips:
            row = int(i / 8)
            col = i % 8
            self.pieces[row,col] *= -1

        self.pieces[move.row,move.col] = 1 if color == Color.BLACK else -1

    def get_moves(self, color: Color):
        c = 1 if color == Color.BLACK else -1

        moves = set()
        for i in range(8):
            row       = extract_row(self.pieces, i, 0)
            row_idx   = extract_row(piece_idx, i, 0)
            col       = extract_col(self.pieces, 0, i)
            col_idx   = extract_col(piece_idx, 0, i)
            diag      = extract_diag(self.pieces, i, 0)
            diag_idx  = extract_diag(piece_idx, i, 0)
            adiag     = extract_adiag(self.pieces, i, 0)
            adiag_idx = extract_adiag(piece_idx, i, 0)

            moves.update(get_moves(row, row_idx, c))
            moves.update(get_moves(col, col_idx, c))
            moves.update(get_moves(diag, diag_idx, c))
            moves.update(get_moves(adiag, adiag_idx, c))

        for i in range(8):
            diag      = extract_diag(self.pieces, 0, i)
            diag_idx  = extract_diag(piece_idx, 0, i)
            adiag     = extract_adiag(self.pieces, 7, i)
            adiag_idx = extract_adiag(piece_idx, 7, i)

            moves.update(get_moves(diag, diag_idx, c))
            moves.update(get_moves(adiag, adiag_idx, c))

        return set([Move(m // 8, m % 8) for m in moves])

    def old_get_moves(self, color: Color):
        moves = set()

        for row in range(8):
            for col in range(8):
                m = Move(row, col)

                if self.pieces[m.row,m.col] == 0 and len(self.get_flips(m, color)) != 0:
                    moves.add(m)

        return moves

    def count_pieces(self, color: Color):
        n = 0

        for p in self.pieces.flatten():
            if (p == 1 and color == Color.BLACK) or (p == -1 and color == Color.WHITE):
                n += 1

        return n

    def piece_list(self):
        return self.pieces.tolist()

    def pretty(self):
        ret = "  a b c d e f g h\n"
        for i in range(len(self.pieces)):
            ret += str(i) + " "
            for j in range(len(self.pieces[i])):
                if self.pieces[i][j] == 1:
                    ret += "# "
                elif self.pieces[i][j] == -1:
                    ret += "O "
                else:
                    ret += "_ "
            ret += "\n"

        return ret

    def to_str(self, color: Color):
        ret = ""
        for row in self.pieces:
            for p in row:
                # print with X as our pieces, O for opponent
                if color == Color.WHITE:
                    p *= -1

                if p == 1:
                    ret += "X"
                elif p == -1:
                    ret += "O"
                else:
                    ret += "-"
        return ret

    def __repr__(self):
        ret = ""
        for row in self.pieces:
            for p in row:
                if p == 1:
                    ret += "X"
                elif p == -1:
                    ret += "O"
                else:
                    ret += "-"
        return ret


class InvalidMove(ValueError):
    """
    Exception thrown by the board when an invalid move is played.
    """
    pass
