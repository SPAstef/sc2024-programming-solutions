from sys import argv


def sigma(word):
    new_word = 0
    # first move the two most significant bits of nibble 0 and 3
    new_word |= (word & 0b1100000000001100) >> 1  # 0, 1, C, D
    # now move the rest of the bits
    new_word |= (word & 0x2000) >> 6  # 2
    new_word |= (word & 0x1000) >> 8  # 3
    new_word |= (word & 0x0C00) >> 5  # 4, 5
    new_word |= (word & 0x0200) << 6  # 6
    new_word |= (word & 0x0100) << 4  # 7
    new_word |= (word & 0x00C0) << 3  # 8, 9
    new_word |= (word & 0x0020) >> 2  # A
    new_word |= (word & 0x0010) >> 4  # B
    new_word |= (word & 0x0002) << 10  # E
    new_word |= (word & 0x0001) << 8  # E

    return new_word


def sbox(word):
    SBOX = (0xE, 0xB, 0x4, 0x6, 0xA, 0xD, 0x7, 0x0,
            0x3, 0x8, 0xF, 0xC, 0x5, 0x9, 0x1, 0x2)

    new_word = 0

    new_word |= SBOX[(word >> 0) & 0xF] << 0
    new_word |= SBOX[(word >> 4) & 0xF] << 4
    new_word |= SBOX[(word >> 8) & 0xF] << 8
    new_word |= SBOX[(word >> 12) & 0xF] << 12

    return new_word


def main():
    if len(argv) < 2:
        print(f"Usage: {argv[0]} <xxxx>")
        exit(1)

    x = int(argv[1], 16) & 0xFFFF
    print(f"{x:04x} -> {sigma(x):04x}")


if __name__ == "__main__":
    main()
