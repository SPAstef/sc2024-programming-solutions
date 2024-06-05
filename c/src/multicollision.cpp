#include "string_utils.hpp"
#include <array>
#include <map>
#include <print>
#include <random>
#include <ranges>
#include <span>
#include <vector>


static constexpr size_t COMPRESS_IN = 4;
static constexpr size_t COMPRESS_OUT = 2;

static constexpr size_t IN_BITS [[maybe_unused]] = COMPRESS_IN * CHAR_BIT;
static constexpr size_t OUT_BITS = COMPRESS_OUT * CHAR_BIT;

static constexpr size_t H1_CHAIN_N = OUT_BITS;
static constexpr size_t H1_EXPQ = H1_CHAIN_N * (1ULL << (H1_CHAIN_N / 2));

static constexpr size_t H2_PATH_LN = OUT_BITS / 2;
static constexpr size_t H2_PATH_N = 1ULL << H2_PATH_LN;
static constexpr size_t H2_EXPQ = H1_CHAIN_N * (1ULL << (OUT_BITS / 2));

static size_t total_queries = 0;

template<size_t msg_sz, size_t dig_sz>
class IdealCompression
{
    static_assert(msg_sz > dig_sz, "Not a compression function! (msg_sz <= dig_sz)");

public:
    static constexpr size_t MSG_SZ = msg_sz;
    static constexpr size_t DIG_SZ = dig_sz;

    using Msg = std::array<uint8_t, MSG_SZ>;
    using Dig = std::array<uint8_t, DIG_SZ>;

private:
    std::mt19937 rng{std::random_device{}()};
    std::map<Msg, Dig> tab;

public:
    Dig hash(const Msg &msg)
    {
        if (tab.contains(msg))
            return tab[msg];

        Dig dig;

        std::ranges::generate(dig, std::ref(rng));
        tab.emplace(msg, dig);

        return dig;
    }
};

template<auto &comp>
class IdealCompressionProxy
{
public:
    using Comp = std::decay_t<decltype(comp)>;
    using Msg = Comp::Msg;
    using Dig = Comp::Dig;

    static constexpr size_t MSG_SZ = Comp::MSG_SZ;
    static constexpr size_t DIG_SZ = Comp::DIG_SZ;

    IdealCompressionProxy() = delete;

    static Dig hash(const Msg &msg) { return comp.hash(msg); }
};


template<typename Comp_t>
class MerkleDamgard
{
public:
    using Comp = Comp_t;
    using CMsg = Comp::Msg;
    using Dig = Comp::Dig;

    static constexpr size_t DIG_SZ = Comp::DIG_SZ;
    static constexpr size_t BLK_SZ = Comp::MSG_SZ - DIG_SZ;
    static constexpr Dig IV{};

    using Blk = std::array<uint8_t, BLK_SZ>;
    using Msg = std::vector<Blk>;

    MerkleDamgard() = delete;

    static Dig hash(const Msg &msg)
    {
        Dig dig{IV};

        for (size_t i = 0; i < msg.size(); ++i)
        {
            CMsg cmsg{};

            std::ranges::copy(dig, cmsg.begin());
            std::ranges::copy(msg[i], cmsg.begin() + dig.size());
            dig = Comp::hash(cmsg);
        }

        return dig;
    }
};

template<typename... Hashes>
class ChainHash
{
public:
    static constexpr size_t BLK_SZ = std::min(Hashes::BLK_SZ...);
    static constexpr size_t DIG_SZ = (Hashes::DIG_SZ + ...);

    using Blk = std::tuple_element_t<0, std::tuple<typename Hashes::Blk...>>;
    using Msg = std::vector<Blk>;
    using Dig = std::array<uint8_t, DIG_SZ>;

    ChainHash() = delete;

    static Dig hash(const Msg &msg)
    {
        Dig dig{};
        auto it = dig.begin();

        ((std::ranges::copy(Hashes::hash(msg), it), it += Hashes::DIG_SZ), ...);

        return dig;
    }
};

template<typename T>
using Chain = std::vector<std::pair<T, T>>;

template<typename Hash>
std::pair<typename Hash::Blk, typename Hash::Blk> find_iv_comp_coll(const typename Hash::Dig &iv)
{
    using Blk = typename Hash::Blk;
    using Dig = typename Hash::Dig;
    using CMsg = typename Hash::CMsg;
    using Comp = typename Hash::Comp;

    std::map<Dig, Blk> queries;
    CMsg msg{};

    std::ranges::copy(iv, msg.begin());

    for (size_t q = 0;; ++q)
    {
        Blk blk;

        std::ranges::generate(blk, [&, i = 0] mutable { return (q >> (8 * i++)) & 0xFF; });
        std::ranges::copy(blk, msg.begin() + iv.size());

        Dig dig{Comp::hash(msg)};

        if (queries.contains(dig))
        {
            total_queries += q;

            return {queries[dig], blk};
        }

        queries.emplace(dig, blk);
    }
}

template<typename Hash>
Chain<typename Hash::Blk> find_hash_multicoll(size_t t = 1)
{
    using Comp = typename Hash::Comp;
    using Dig = typename Hash::Dig;
    using Blk = typename Hash::Blk;
    using CMsg = typename Hash::CMsg;

    Chain<Blk> chain;

    Dig dig{Hash::IV};
    for (size_t i = 0; i < t; ++i)
    {
        chain.emplace_back(find_iv_comp_coll<Hash>(dig));

        CMsg msg{};
        const Blk &blk = chain.back().first;

        std::ranges::copy(dig, msg.begin());
        std::ranges::copy(blk, msg.begin() + dig.size());
        dig = Comp::hash(msg);
    }

    return chain;
}

template<typename Hash>
std::pair<size_t, size_t> find_coll_path(const Chain<typename Hash::Blk> &chain)
{
    using Dig = typename Hash::Dig;
    using Msg = typename Hash::Msg;

    std::map<Dig, size_t> queries;

    for (size_t i = 0; i < 1ULL << chain.size(); ++i)
    {
        Msg msg(chain.size());

        std::ranges::generate(msg, [&, j = 0] mutable
                              { return i >> j & 1 ? chain[j++].second : chain[j++].first; });

        Dig dig{Hash::hash(msg)};

        total_queries += chain.size();
        if (queries.contains(dig))
            return {queries[dig], i};

        queries.emplace(dig, i);
    }

    return {};
}

template<typename Hash>
std::vector<size_t> find_coll_paths(const Chain<typename Hash::Blk> &chain)
{
    using Dig = typename Hash::Dig;
    using Msg = typename Hash::Msg;

    std::map<Dig, std::vector<size_t>> queries;

    for (size_t i = 0, n = 1ULL << chain.size(); i < n; ++i)
    {
        Msg msg(chain.size());

        std::ranges::generate(msg, [&, j = 0] mutable
                              { return (i >> j) & 1 ? chain[j++].second : chain[j++].first; });

        Dig dig{Hash::hash(msg)};

        total_queries += chain.size();
        if (queries.contains(dig))
            queries[dig].emplace_back(i);
        else
            queries[dig] = {i};

        if (queries[dig].size() == H2_PATH_N)
            return queries[dig];
    }

    std::println("Could not find enough collision, returing best candidate set...");

    return std::ranges::max_element(queries, [](auto &&x, auto &&y)
                                    { return x.second.size() < y.second.size(); })
        ->second;
}

int main()
{
    static IdealCompression<COMPRESS_IN, COMPRESS_OUT> comp1, comp2, comp3;

    using Hash1 = MerkleDamgard<IdealCompressionProxy<comp1>>;
    using Hash2 = MerkleDamgard<IdealCompressionProxy<comp2>>;
    using Hash3 = MerkleDamgard<IdealCompressionProxy<comp3>>;
    using Hash = ChainHash<Hash1, Hash2>;

    std::println("Looking for 2^{} collisions in H1...", H1_CHAIN_N);

    Chain<Hash1::Blk> chain{find_hash_multicoll<Hash1>(H1_CHAIN_N)};

    if (std::ranges::any_of(chain, [&](auto &&x) { return x.first == x.second; }))
    {
        std::println("The two chains are equal!");
        return 0;
    }

    std::println("Found in {}/{} queries!", total_queries, H1_EXPQ);
    total_queries = 0;

    std::println("Looking for a common collision with H2... ");

    std::vector<size_t> paths{find_coll_paths<Hash2>(chain)};
    std::ranges::sort(paths);

    if (paths.empty())
    {
        std::println("Could not find a collision!");
        return 0;
    }
    std::println("Found {} collisions in {}/{} queries!", paths.size(), total_queries, H2_EXPQ);
    total_queries = 0;

    bool still_same = true;
    bool all_diff = true;

    Hash::Msg msg0(chain.size());
    std::ranges::generate(msg0, [&, j = 0] mutable
                          { return paths[0] >> j & 1 ? chain[j++].second : chain[j++].first; });

    Hash::Dig dig0{Hash::hash(msg0)};

    for (size_t i = 1; i < paths.size(); ++i)
    {
        Hash::Msg msg(chain.size());
        std::ranges::generate(msg, [&, j = 0] mutable
                              { return paths[i] >> j & 1 ? chain[j++].second : chain[j++].first; });

        all_diff &= paths[i - 1] != paths[i];
        still_same &= Hash::hash(msg) == dig0;

        if (!all_diff)
        {
            std::println("Bad collision at {}: {:016x} with {:016x}", i, paths[i - 1], paths[i]);
            break;
        }
    }

    if (all_diff && still_same)
        std::println("All different, but still same!");
    else if (all_diff && !still_same)
        std::println("All different, but different!");
    else if (!all_diff && still_same)
        std::println("Same same, but still same!");
    else if (!all_diff && !still_same)
        std::println("Same same, but different!");

    /*
    for (size_t i = 0; i < paths.size(); ++i)
    {
        Hash::Msg msg(chain.size());
        std::ranges::generate(msg, [&, j = 0] mutable
                              { return paths[i] >> j & 1 ? chain[j++].second : chain[j++].first; });

        std::println("Message{}: {}", i, hexdump(msg));
        Hash::Dig dig{Hash::hash(msg)};

        std::print("Digest{}: ", i);
        for (auto &&x : dig)
            std::print("{:02x}", x);
        std::println("");
    }*/

    return 0;
}
