#include "rand.h"
#include "string_utils.h"
#include <inttypes.h>

typedef enum
{
    CLUBS,
    DIAMONDS,
    HEARTS,
    SPADES,
    SUIT_N,
} Suit;

typedef enum
{
    ACE,
    TWO,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
    NINE,
    TEN,
    JACK,
    QUEEN,
    KING,
    RANK_N,
} Rank;

enum
{
    CARDS_N = 52,
};

typedef struct card
{
    Suit suit;
    Rank rank;
} card_t;

typedef struct
{
    card_t cards[CARDS_N];
} deck_t;

typedef struct
{
    card_t cards[CARDS_N / 2];
} Alice;

typedef struct
{
    card_t cards[CARDS_N / 2];
} Bob;

Rank rank_from_int(uint8_t i)
{
    return (Rank)i;
}

uint8_t rank_to_int(Rank r)
{
    return (uint8_t)r;
}

Rank next_rank(Rank r)
{
    return rank_from_int(rank_to_int(r) + 1);
}

Suit suit_from_int(uint8_t i)
{
    return (Suit)i;
}

uint8_t suit_to_int(Suit s)
{
    return (uint8_t)s;
}

Suit next_suit(Suit s)
{
    return suit_from_int(suit_to_int(s) + 1);
}

card_t card_from_int(uint8_t i)
{
    return (card_t){.suit = suit_from_int(i / RANK_N), .rank = rank_from_int(i % RANK_N)};
}

uint8_t card_to_int(card_t c)
{
    return suit_to_int(c.suit) * RANK_N + rank_to_int(c.rank);
}

card_t next_card(card_t c)
{
    return card_from_int(card_to_int(c) + 1);
}

void deck_shuffle(deck_t *deck)
{
    for (size_t i = 0; i < CARDS_N; ++i)
    {
        size_t j = prand32(i, CARDS_N - 1);
        card_t t = deck->cards[i];
        deck->cards[i] = deck->cards[j];
        deck->cards[j] = t;
    }
}

uint64_t binom(uint64_t n, uint64_t k)
{
    if (k > n)
        return 0;

    if (k > n / 2)
        k = n - k;

    uint64_t r = 1;

    for (uint64_t d = 1; d <= k; ++d, --n)
    {
        r *= n;
        r /= d;
    }

    return r;
}

void sort_cards(card_t *cards, size_t n)
{
    // we use selection sort because it's simple and the number of cards is small
    for (size_t i = 0; i < n; ++i)
    {
        uint8_t v_i = card_to_int(cards[i]);

        for (size_t j = i + 1; j < n; ++j)
        {
            uint8_t v_j = card_to_int(cards[j]);

            if (v_j < v_i)
            {
                card_t t = cards[i];
                cards[i] = cards[j];
                cards[j] = t;
                v_i = v_j;
            }
        }
    }
}

uint64_t alice_getkey(Alice alice)
{
    uint64_t key = 0;

    sort_cards(alice.cards, CARDS_N / 2);

    for (size_t i = 0; i < CARDS_N / 2; ++i)
    {
        uint8_t v = card_to_int(alice.cards[i]);

        key += binom(v, i + 1);
    }

    return key & 0xFFFFFFFFFFFF;
}

uint64_t bob_getkey(Bob bob)
{
    Alice alice;

    sort_cards(bob.cards, CARDS_N / 2);

    uint8_t old_bc = card_to_int(bob.cards[0]);
    uint8_t k = 0;

    for (uint8_t j = 0; j < old_bc; ++j)
        alice.cards[k++] = card_from_int(j);

    for (size_t i = 1; i < CARDS_N / 2; ++i)
    {
        uint8_t new_bc = card_to_int(bob.cards[i]);

        for (uint8_t j = old_bc + 1; j < new_bc; ++j)
            alice.cards[k++] = card_from_int(j);

        old_bc = new_bc;
    }

    for (uint8_t j = old_bc + 1; j < CARDS_N; ++j)
        alice.cards[k++] = card_from_int(j);

    return alice_getkey(alice);
}

uint64_t encrypt(Alice alice, uint64_t msg, uint64_t key)
{
    return msg ^ key;
}

uint64_t decrypt(Bob bob, uint64_t cip, uint64_t key)
{
    return cip ^ key;
}


int main()
{
    deck_t deck;

    for (size_t i = 0; i < CARDS_N; ++i)
        deck.cards[i] = card_from_int(i);

    deck_shuffle(&deck);

    Alice alice;
    Bob bob;

    for (size_t i = 0; i < CARDS_N / 2; i++)
    {
        alice.cards[i] = deck.cards[i];
        bob.cards[i] = deck.cards[i + CARDS_N / 2];
    }

    uint64_t msg = prand64(0, 0xFFFFFFFFFFFF); // 48-bit message
    uint64_t alice_key = alice_getkey(alice);
    uint64_t bob_key = bob_getkey(bob);
    uint64_t cip = encrypt(alice, msg, alice_key);
    uint64_t dec = decrypt(bob, cip, bob_key);

    printf("Alice's key: %012" PRIx64 "\n", alice_key);
    printf("Bob's key: %012" PRIx64 "\n", bob_key);
    printf("Plaintext: %012" PRIx64 "\n", msg);
    printf("Ciphertext: %012" PRIx64 "\n", cip);
    printf("Decrypted: %012" PRIx64 "\n", dec);

    return 0;
}
