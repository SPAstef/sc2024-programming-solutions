from sys import argv


def main():
    if len(argv) < 2:
        print(f"Usage: {argv[0]} <[s_1, ..., s_n]>")

    sbox = list(map(int, argv[1][1:-1].split(",")))
    print(f"S-Box: {sbox}")

    N = len(sbox)

    ddt = [[0 for _ in range(N)] for _ in range(N)]

    for x1 in range(N):
        for x2 in range(N):
            ddt[x1 ^ x2][sbox[x1] ^ sbox[x2]] += 1

    print("DDT:")
    for row in ddt:
        print(row)
    print()

    lat = [[0 for _ in range(N)] for _ in range(N)]

    for x1 in range(N):
        for m_in in range(N):
            for m_out in range(N):
                lat[x1][m_out] += ((x1 & m_in).bit_count() & 1) == ((sbox[m_in]
                                                                     & m_out).bit_count() & 1)
                
    print("LAT:")
    for row in lat:
        print(row)
    print()

if __name__ == "__main__":
    main()
