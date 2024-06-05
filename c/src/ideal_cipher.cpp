#include "span_utils.hpp"
#include "string_utils.h"

#include <algorithm>
#include <array>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include <numeric>


template<size_t key_sz = 16, size_t msg_sz = key_sz>
class IdealCipher
{
public:
    static constexpr size_t KEY_SIZE = key_sz;
    static constexpr size_t MSG_SIZE = msg_sz;

    using Key = std::array<uint8_t, KEY_SIZE>;
    using Msg = std::array<uint8_t, MSG_SIZE>;
    using KeyS = std::span<const uint8_t, KEY_SIZE>;
    using MsgS = std::span<const uint8_t, MSG_SIZE>;

private:
    static inline std::map<Key, std::pair<std::map<Msg, Msg>, std::map<Msg, Msg>>> table;
    static inline std::random_device rng;

public:
    static Msg encrypt(KeyS key, MsgS pln_v)
    {
        Msg pln{range_from_span<Msg>(pln_v)};
        auto &[pln_tab, cip_tab] = table.try_emplace(range_from_span<Key>(key)).first->second;

        if (pln_tab.contains(pln))
            return pln_tab[pln];

        Msg cip;

        do
            std::ranges::generate(cip, std::ref(rng));
        while (std::ranges::any_of(pln_tab, [&](auto &&pair) { return pair.second == cip; }));

        pln_tab.emplace(pln, cip);
        cip_tab.emplace(cip, pln);

        return cip;
    }

    static Msg decrypt(KeyS key, MsgS cip_v)
    {
        Msg cip{range_from_span<Msg>(cip_v)};

        auto &[pln_tab, cip_tab] = table.try_emplace(range_from_span<Key>(key)).first->second;

        if (cip_tab.contains(cip))
            return cip_tab[cip];

        Msg pln;

        do
            std::ranges::generate(pln, std::ref(rng));
        while (std::ranges::any_of(cip_tab, [&](auto &&pair) { return pair.second == pln; }));

        cip_tab.emplace(cip, pln);
        pln_tab.emplace(pln, cip);

        return pln;
    }

    static void dump_state(std::ofstream &fs)
    {
        size_t sz = table.size();

        fs.write(reinterpret_cast<const char *>(&sz), sizeof(sz));
        for (auto &[key, pair] : table)
        {
            auto &pln_tab = pair.first;

            fs.write(reinterpret_cast<const char *>(key.data()), key.size());

            sz = pln_tab.size();
            fs.write(reinterpret_cast<const char *>(&sz), sizeof(sz));
            for (auto &[pln, cip] : pln_tab)
            {
                fs.write(reinterpret_cast<const char *>(pln.data()), pln.size());
                fs.write(reinterpret_cast<const char *>(cip.data()), cip.size());
            }
        }
    }

    static void dump_state(std::ofstream &&fs) { dump_state(fs); }

    static void load_state(std::ifstream &fs)
    {
        size_t sz = 0;

        fs.read(reinterpret_cast<char *>(&sz), sizeof(sz));
        for (size_t i = 0; i < sz; ++i)
        {
            Key key{};
            fs.read(reinterpret_cast<char *>(key.data()), key.size());

            auto &[pln_tab, cip_tab] = table.try_emplace(key).first->second;

            fs.read(reinterpret_cast<char *>(&sz), sizeof(sz));
            for (size_t j = 0; j < sz; ++j)
            {
                Msg pln{}, cip{};

                fs.read(reinterpret_cast<char *>(pln.data()), pln.size());
                fs.read(reinterpret_cast<char *>(cip.data()), cip.size());

                pln_tab.emplace(pln, cip);
                cip_tab.emplace(cip, pln);
            }
        }
    }

    static void load_state(std::ifstream &&fs) { load_state(fs); }

    IdealCipher() = delete;
};

int main(int argc, char **argv)
{
    using E = IdealCipher<8>;

    if (argc < 4)
    {
        std::cerr << "Usage: " << argv[0] << "<b> <key> <msg>\n";
        exit(EXIT_FAILURE);
    }

    bool b = std::stoi(argv[1]);
    E::Key key{};
    E::Msg msg{};

    hexload(key.data(), key.size(), argv[2]);
    hexload(msg.data(), msg.size(), argv[3]);

    E::load_state(std::ifstream{"ideal_cipher.dat", std::ios::binary});

    E::Msg cip = b ? E::decrypt(key, msg) : E::encrypt(key, msg);

    hexprint(stdout, cip.data(), cip.size());

    E::dump_state(std::ofstream{"ideal_cipher.dat", std::ios::binary});

    return 0;
}
