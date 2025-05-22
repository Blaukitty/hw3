#include <bits/stdc++.h>
using namespace std;

static vector<uint8_t> read_all(const string& path) {
    ifstream in(path, ios::binary);
    if (!in) throw runtime_error("cannot open input file");
    return vector<uint8_t>((istreambuf_iterator<char>(in)), {});
}

struct Code {
    uint8_t  sym = 0;
    uint32_t bits = 0;   // код, младшие разряды – младшие биты
    uint8_t  len  = 0;
    string bits_str() const {            // для вывода в .dict
        string s(len, '0');
        for (int i = len - 1; i >= 0; --i)
            s[len - 1 - i] = ((bits >> i) & 1) ? '1' : '0';
        return s;
    }
};

class BitWriter {
    vector<uint8_t> buf_;
    uint8_t cur_ = 0;        // собираем биты слева-направо
    int     filled_ = 0;
public:
    void write_bits(uint32_t bits, uint8_t len) {
        for (int i = len - 1; i >= 0; --i) {
            cur_ = (cur_ << 1) | ((bits >> i) & 1);
            if (++filled_ == 8) { buf_.push_back(cur_); cur_ = filled_ = 0; }
        }
    }
    vector<uint8_t> finish() {
        if (filled_) { cur_ <<= (8 - filled_); buf_.push_back(cur_); }
        return buf_;
    }
};

static void shannon_fano(vector<pair<uint8_t, uint32_t>>& a,
                         size_t l, size_t r,
                         vector<Code>& out,
                         uint32_t prefix, uint8_t depth)
{
    if (r - l == 1) {                     // лист
        out[a[l].first] = {a[l].first, prefix, depth};
        return;
    }
    uint64_t sum = 0; for (size_t i=l;i<r;++i) sum += a[i].second;
    uint64_t acc = 0; size_t m = l;
    while (m < r && acc + a[m].second <= sum / 2) acc += a[m++].second;
    if (m == l) ++m;                      // чтобы не зациклиться
    shannon_fano(a, l, m, out, (prefix << 1),     depth + 1);
    shannon_fano(a, m, r, out, (prefix << 1) | 1, depth + 1);
}

void encode_file(const string& inName,
                 const string& dataOut,
                 const string& dictOut)
{
    vector<uint8_t> data = read_all(inName);

    // 1. частоты
    array<uint32_t, 256> freq{};                      // == {0}
    for (uint8_t b : data) ++freq[b];

    // 2. строим коды
    vector<pair<uint8_t,uint32_t>> sym;
    for (int i = 0; i < 256; ++i) if (freq[i]) sym.push_back({uint8_t(i),freq[i]});
    if (sym.empty()) throw runtime_error("empty input");
    sort(sym.begin(), sym.end(),
         [](auto& x, auto& y){ return x.second > y.second; });

    vector<Code> codes(256);
    shannon_fano(sym, 0, sym.size(), codes, 0, 0);

    // 3. пишем словарь
    ofstream dict(dictOut);
    dict << sym.size() << '\n';
    for (auto& s : sym) {
        auto& c = codes[s.first];
        dict << int(c.sym) << ' ' << int(c.len) << ' ' << c.bits_str() << '\n';
    }

    // 4. сжимаем поток
    BitWriter bw;
    for (uint8_t b : data) {
        auto& c = codes[b];
        bw.write_bits(c.bits, c.len);
    }
    vector<uint8_t> packed = bw.finish();

    // 5. пишем data-файл (<orig-size><bitstream>)
    ofstream out(dataOut, ios::binary);
    uint32_t orig = data.size();
    out.put(orig & 0xFF);
    out.put((orig >> 8) & 0xFF);
    out.put((orig >> 16) & 0xFF);
    out.put((orig >> 24) & 0xFF);
    out.write(reinterpret_cast<char*>(packed.data()), packed.size());
}
