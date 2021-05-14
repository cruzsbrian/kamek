import numpy as np
from scipy.sparse import csr_matrix
from sklearn.linear_model import LogisticRegression
from sklearn.preprocessing import MaxAbsScaler
from progress.bar import IncrementalBar
import argparse


def score_to_winloss(score):
    return 1 if score > 0 else 0


MASK_BASE = [
    0, 0, 0, 0,
    1, 1, 1, 1,
    2, 2, 2, 2,
    3, 3, 3, 3,
    4, 4,
    5, 5, 5, 5,
    6, 6, 6, 6,
    7, 7, 7, 7,
    8, 8, 8, 8,
]

N_INSTANCES = [ 81, 243, 729, 2187, 6561, 6561, 6561, 6561, 6561 ]
START_POS = [sum(N_INSTANCES[:i]) for i in range(len(N_INSTANCES))]


def train(filename):

    # Get a quick line count
    n_trials = sum(1 for i in open(filename, "rb"))
    n_params = sum(N_INSTANCES)

    bar = IncrementalBar("Constructing matrix...", max=n_trials)

    data = []
    indices = []
    indptr = []
    scores = []

    for line in open(filename):
        line_data = [int(x) for x in line.split()]

        indptr.append(len(indices))

        for j in range(len(line_data) - 2):
            instance = line_data[j + 1]
            base = MASK_BASE[j]
            start_pos = START_POS[base]

            indices.append(start_pos + instance)
            data.append(1)

        scores.append(score_to_winloss(line_data[-1]))

        bar.next()

    indptr.append(len(indices))

    a = csr_matrix((data, indices, indptr), shape=(n_trials, n_params))
    b = np.array(scores)

    bar.finish()

    print("Scaling...")
    scaler = MaxAbsScaler()
    a_std = scaler.fit_transform(a)

    print("Fitting...")
    model = LogisticRegression(solver='saga', fit_intercept=False)
    model.fit(a_std, b)

    print("Model score:", model.score(a_std, b))

    coefs_rescaled = model.coef_ / scaler.scale_

    return np.rint(coefs_rescaled * 500)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("filename", type=str)

    args = parser.parse_args()

    weights = train(args.filename)[0]

    print(weights)
    print("max:", np.max(weights))
    print("min:", np.min(weights))

    print("Writing weights to file...")
    with open("weights.txt", "w") as outfile:
        n = 0
        for w in weights:
            outfile.write(str(int(w)))
            n += 1
            if n % 16 == 0:
                outfile.write(",\n")
            else:
                outfile.write(", ")
