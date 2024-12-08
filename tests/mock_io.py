def load_input_txt(fpath):
    with open(fpath, "r") as f:
        L, Q = map(int, f.readline().split())
        C, X = list(), list()
        for q in range(Q):
            c, x = map(int, f.readline().split())
            C.append(c)
            X.append(x)
    return L, Q, C, X


def load_output_txt(fpath):
    with open(fpath, "r") as f:
        return [int(val) for val in f.read().splitlines()]
