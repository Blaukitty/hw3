#include <iostream>
#include <string>
using namespace std;

void encode_file(const string&, const string&, const string&);
void decode_file(const string&, const string&, const string&);

int main(int argc, char* argv[])
{
    if (argc < 2) {
        cerr << "Usage:\n"
                "  encode <in> <out.shn> <out.dict>\n"
                "  decode <in.shn> <in.dict> <out>\n";
        return 1;
    }
    string mode = argv[1];
    try {
        if (mode == "encode") {
            if (argc != 5) throw runtime_error("encode needs 3 args");
            encode_file(argv[2], argv[3], argv[4]);
        } else if (mode == "decode") {
            if (argc != 5) throw runtime_error("decode needs 3 args");
            decode_file(argv[2], argv[3], argv[4]);
        } else {
            throw runtime_error("unknown mode: " + mode);
        }
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
