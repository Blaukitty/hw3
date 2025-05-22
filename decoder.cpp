#include <bits/stdc++.h>
using namespace std;

static vector<uint8_t> read_bin(const string& path) {
    ifstream in(path, ios::binary);
    if (!in) throw runtime_error("cannot open input file");
    return vector<uint8_t>((istreambuf_iterator<char>(in)), {});
}

struct Node { int left = -1, right = -1, sym = -1; };

class BitReader {
    const vector<uint8_t>& buf_;
    size_t byte_ = 0;
    int bit_ = 0;               
public:
    explicit BitReader(const vector<uint8_t>& b) : buf_(b) {}
    bool read_bit(bool& bit) {
        if (byte_ >= buf_.size()) return false;
        bit = (buf_[byte_] >> (7 - bit_)) & 1;
        if (++bit_ == 8) { bit_ = 0; ++byte_; }
        return true;
    }
};

void decode_file(const string& dataIn,
                 const string& dictIn,
                 const string& outName)
{
    // 1. строим три из словаря
    ifstream dict(dictIn);
    size_t nSym;  dict >> nSym;
    vector<Node> trie(1);           // корень = 0

    for (size_t i = 0; i < nSym; ++i) {
        int sym, len; string code;
        dict >> sym >> len >> code;
        int v = 0;
        for (char b : code) {
            if (b == '0') {
                if (trie[v].left == -1) {
                    trie[v].left = static_cast<int>(trie.size());
                    trie.emplace_back();
                }
                v = trie[v].left;
            } else { // '1'
                if (trie[v].right == -1) {
                    trie[v].right = static_cast<int>(trie.size());
                    trie.emplace_back();
                }
                v = trie[v].right;
            }    
        }
        trie[v].sym = sym;
    }

    // 2. читаем сжатый поток
    vector<uint8_t> raw = read_bin(dataIn);
    if (raw.size() < 4) throw runtime_error("corrupted data file");

    uint32_t orig =  uint32_t(raw[0])
                   | uint32_t(raw[1]) << 8
                   | uint32_t(raw[2]) << 16
                   | uint32_t(raw[3]) << 24;
    vector<uint8_t> body(raw.begin() + 4, raw.end());

    BitReader br(body);
    vector<uint8_t> out;
    out.reserve(orig);

    bool bit;
    int v = 0;
    while (out.size() < orig && br.read_bit(bit)) {
        v = bit ? trie[v].right : trie[v].left;
        if (v == -1) throw runtime_error("bad code stream");
        if (trie[v].sym != -1) { out.push_back(uint8_t(trie[v].sym)); v = 0; }
    }
    if (out.size() != orig) throw runtime_error("unexpected EOF");

    ofstream fout(outName, ios::binary);
    fout.write(reinterpret_cast<char*>(out.data()), out.size());
}
