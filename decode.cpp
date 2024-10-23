#include "decode.h"

std::unordered_map<EncodingType, DBitset> DecodeNormalizedForm(std::vector<EncodingType> &alph,
                                                               std::vector<size_t> &lens) {
    std::unordered_map<EncodingType, DBitset> result;
    uint64_t code = 0;
    for (size_t i = 0; i < alph.size(); ++i) {
        result[alph[i]] = DBitset(lens[i], ReverseCode(code, lens[i]));
        if (i + 1 < alph.size()) {
            code = (code + 1) << (lens[i + 1] - lens[i]);
        }
    }
    return result;
}

EncodingType GetNextSymbol(Reader &reader, TrieNode *&node, TrieNode *&head) {
    while (!node->is_terminated_) {
        bool bit = reader.Read(1)[0];
        if (bit) {
            if (!node->right_) {
                throw DecodeException("incorrect bit sequence.");
            }
            node = node->right_;
        } else {
            if (!node->left_) {
                throw DecodeException("incorrect bit sequence.");
            }
            node = node->left_;
        }
    }
    EncodingType result = node->ch_;
    node = head;
    return result;
}

void Decode(std::string &file_name) {
    std::ifstream in(file_name.c_str(), std::ios::binary | std::ios_base::in);
    Reader reader(in);
    do {
        size_t alph_size = ConvertBitsetToEncodingType(reader.Read(SpecialByteSize));
        std::vector<EncodingType> alph;
        for (size_t i = 0; i < alph_size; i++) {
            alph.emplace_back(ConvertBitsetToEncodingType(reader.Read(SpecialByteSize)));
        }
        std::vector<size_t> lens;
        size_t sum = 0;
        size_t len = 1;
        do {
            size_t count = ConvertBitsetToEncodingType(reader.Read(SpecialByteSize));
            for (size_t i = 0; i < count; ++i) {
                lens.push_back(len);
            }
            sum += count;
            len++;
        } while (sum < alph_size);
        std::unordered_map<EncodingType, DBitset> normalized_dict = DecodeNormalizedForm(alph, lens);

        TrieNode *head = Trie::BuildTrieByPaths(normalized_dict);
        TrieNode *node = head;

        std::string output_file_name;
        do {
            EncodingType symbol = GetNextSymbol(reader, node, head);
            if (symbol == FilenameEnd) {
                break;
            }
            output_file_name.push_back(static_cast<char>(symbol));
        } while (true);

        std::ofstream out(output_file_name, std::ios::binary | std::ios::out);
        Writer writer(out);

        do {
            EncodingType symbol = GetNextSymbol(reader, node, head);
            if (symbol == OneMoreFile) {
                Trie::Clear(head);
                writer.Flush();
                out.close();
                break;
            } else if (symbol == ArchiveEnd) {
                Trie::Clear(head);
                writer.Flush();
                out.close();
                return;
            } else {
                writer.Write(ConvertEncodingTypeToBitset(symbol));
            }
        } while (true);

    } while (true);
}