import numpy as np
from sklearn.linear_model import LogisticRegression

import argparse


def score_to_winloss(score):
    return 1 if score > 0 else 0


def split_by_empties(filename):
    print("Reading file...")

    blocks = {}

    with open(filename) as f:
        for line_str in f:
            line = line_str.split()

            empties = int(line[0])
            game_phase = int((empties - 1) / 6);

            if game_phase in blocks:
                blocks[game_phase].append(line[1:])
            else:
                blocks[game_phase] = [line[1:]]

    return blocks


def train_block(lines):
    n_trials = len(lines)
    n_params = len(lines[0]) - 1

    a = np.zeros((n_trials, n_params), dtype=np.int8)
    b = np.empty(n_trials)

    # print(lines)

    for i in range(n_trials):
        a[i] = [int(x) for x in lines[i][:-1]] # + [1]
        # b[i] = int(lines[i][-1])
        b[i] = score_to_winloss(int(lines[i][-1]))

    # print(n_trials)
    # print(b)

    model = LogisticRegression(solver='liblinear', fit_intercept=False)
    model.fit(a, b)

    print("Model score:", model.score(a, b))

    return np.rint(model.coef_ * 500), model.score(a, b)

    # ainv = np.linalg.pinv(a)
    # result = np.array(np.matmul(ainv, b))

    # mean = np.mean(b)
    # sstot = np.sum((b - mean)**2)

    # f = np.array(np.matmul(a, result))
    # ssres = np.sum((f - b)**2)

    # # print("Weights:", np.rint(result * 50))

    # print("R^2:", 1 - (ssres / sstot))

    # return np.rint(result * 50), 1 - (ssres / sstot)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("filename", type=str)

    args = parser.parse_args()

    blocks = split_by_empties(args.filename)

    for (key, block) in blocks.items():
        print(f"Game phase: {key}")
        print(train_block(block))
        print()
