#define CATCH_CONFIG_MAIN
#include "catch_amalgamated.hpp"

#include <random>
#include <fstream>
#include "encoder.cpp"   
#include "decoder.cpp"

using namespace std;

static vector<uint8_t> str_vec(const string& s) {
    return vector<uint8_t>(s.begin(), s.end());
}

static void roundtrip(const std::vector<uint8_t>& src)
{
    const string fIn  = "tmp_in.bin";
    const string fShn = "tmp_out.shn";
    const string fDic = "tmp_out.dict";
    const string fOut = "tmp_dec.bin";

    ofstream(fIn, ios::binary).write(
        reinterpret_cast<const char*>(src.data()), src.size());

    encode_file(fIn, fShn, fDic);
    decode_file(fShn, fDic, fOut);

    ifstream fin(fOut, ios::binary);
    vector<uint8_t> res{istreambuf_iterator<char>(fin), {} };

    REQUIRE(res == src);
}

TEST_CASE("Small ASCII text")
{
    roundtrip(str_vec("Hello, Shannon–Fano!"));
}

TEST_CASE("All 256 byte values")
{
    vector<uint8_t> v(256);
    iota(v.begin(), v.end(), 0);
    roundtrip(v);
}

TEST_CASE("Random 1 kB blocks")
{
    mt19937_64 rng(123456);
    uniform_int_distribution<int> dist(0, 255);

    for (int t = 0; t < 20; ++t) {
        vector<uint8_t> v(1024);
        for (auto& b : v) b = static_cast<uint8_t>(dist(rng));
        roundtrip(v);
    }
}

TEST_CASE("Empty input")
{
    roundtrip({});
}
