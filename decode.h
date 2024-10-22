
#pragma once
#include "reader.h"
#include "writer.h"
#include "trie.h"
#include "encoderType.h"
#include "exception.h"

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

EncodingType GetNextSymbol(Reader &reader, TrieNode *&node, TrieNode *const &head) {
    while (!node->IsTerminated()) {
        bool bit = reader.Read(1)[0];
        if (bit) {
            if (!node->GetRight()) {
                throw DecodeException("incorrect bit sequence.");
            }
            node = node->GetRight();
        } else {
            if (!node->GetLeft()) {
                throw DecodeException("incorrect bit sequence.");
            }
            node = node->GetLeft();
        }
    }
    EncodingType result = node->GetChar();
    node = head;
    return result;
}

void Decode(const char *file_name) {
    std::ifstream in(file_name, std::ios::binary | std::ios_base::in);
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
            if (symbol == FILENAME_END) {
                break;
            }
            output_file_name.push_back(symbol);
        } while (true);
        std::ofstream out(output_file_name, std::ios::binary | std::ios::out);
        Writer writer(out);
        do {
            EncodingType symbol = GetNextSymbol(reader, node, head);
            if (symbol == ONE_MORE_FILE) {
                writer.Flush();
                out.close();
                break;
            } else if (symbol == ARCHIVE_END) {
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