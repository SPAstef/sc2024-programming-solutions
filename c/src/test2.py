from random import randrange as rng
import numpy as np


def main():
    N = 16
    P = np.zeros(N*N, dtype=np.uint8)

    for i in range(1000):
        P[0] = rng(N)
        while True:
            P[1] = rng(N)
            if P[1] & 1 == 0: # must be odd
                continue

            for j in range(2, P.size):
                P[j] = rng(N)

            if np.sum(P[2::2]) & 1 == 0 and np.sum(P[3::2]) & 1 == 0: # must be even
                break

        print(P)
        S = [3, 14, 1, 10, 4, 9, 5, 6, 8, 11, 15, 2, 13, 12, 0, 7]
        for x in range(N):
            y = P[P.size - 1]
            for j in range(P.size - 2, -1, -1):
                y *= x
                y += P[j]
            S[x] = y % N

        #S = [3, 14, 1, 10, 4, 9, 5, 6, 8, 11, 15, 2, 13, 12, 0, 7]
        #print(S)

        T = np.zeros((N, N), dtype=np.uint8)

        for dx in range(1, len(S)):
            for x1 in range(0, len(S)):
                x2 = x1 ^ dx
                dy = S[x1] ^ S[x2]
                T[dx, dy] += 1
        if T.flatten().max() < 10:
            break

    print(S)
    print("===========")
    print(T)


if __name__ == "__main__":
    main()
