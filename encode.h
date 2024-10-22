#pragma once

#include "heap.h"
#include "trie.h"
#include "reader.h"
#include "writer.h"
#include "encoderType.h"

std::unordered_map<EncodingType, int> GetFrequencyOfBytes(const std::string_view &file_name, Reader &reader) {
    std::unordered_map<EncodingType, int> result;
    for (auto c : file_name) {
        ++result[c];
    }
    while (reader.Size()) {
        auto read = reader.Read(ByteSize);
        ++result[ConvertBitsetToEncodingType(read)];
    }
    result[FILENAME_END] = 1;
    result[ONE_MORE_FILE] = 1;
    result[ARCHIVE_END] = 1;
    return result;
}

std::pair<std::unordered_map<EncodingType, DBitset>, std::vector<std::pair<EncodingType, DBitset>>> Normalize(
    const std::unordered_map<EncodingType, DBitset> &dict) {
    std::vector<std::pair<EncodingType, size_t>> pairs_of_char_and_len;
    for (auto &[a, b] : dict) {
        pairs_of_char_and_len.push_back(std::make_pair(b.size(), a));
    }
    sort(pairs_of_char_and_len.begin(), pairs_of_char_and_len.end());
    std::unordered_map<EncodingType, DBitset> result_dict;
    std::vector<std::pair<EncodingType, DBitset>> result_vec;
    uint64_t code = 0;
    for (size_t i = 0; i < pairs_of_char_and_len.size(); ++i) {
        uint64_t reversed_code = ReverseCode(code, pairs_of_char_and_len[i].first);
        result_dict[pairs_of_char_and_len[i].second] = DBitset(pairs_of_char_and_len[i].first, reversed_code);
        result_vec.push_back(make_pair(pairs_of_char_and_len[i].second, DBitset(pairs_of_char_and_len[i].first, reversed_code)));
        if (i + 1 < pairs_of_char_and_len.size()) {
            code = (code + 1) << (pairs_of_char_and_len[i + 1].first - pairs_of_char_and_len[i].first);
        }
    }
    return std::make_pair(result_dict, result_vec);
}

void EncodeFile(std::string_view file_name, Reader& reader, Writer& writer, bool is_next_file_exists) {
    std::unordered_map<EncodingType, int> byte_frequency = GetFrequencyOfBytes(file_name, reader);
    reader.Refresh();
    auto head = Trie::BuildTrieByFrequences(byte_frequency);
    auto trie_dict = Trie::GetEncodingTable(head);
    Trie::Clear(head);
    auto [normalized_dict, normalized_pairs] = Normalize(trie_dict);
    // SYMBOLS_COUNT
    writer.Write(DBitset(SpecialByteSize, normalized_dict.size()));
    //
    size_t max_len = 0;
    std::unordered_map<size_t, size_t> count_by_len;
    // ALPHABETIC SYMBOLS
    for (const auto &i : normalized_pairs) {
        writer.Write(DBitset(SpecialByteSize, i.first));
        max_len = std::max(max_len, i.second.size());
        count_by_len[i.second.size()]++;
    }
    //
    // COUNT BY LENS
    for (size_t i = 1; i <= max_len; i++) {
        writer.Write(DBitset(SpecialByteSize, count_by_len[i]));
    }
    //
    // FILE NAME
    for (const auto &c : file_name) {
        writer.Write(normalized_dict[c]);
    }
    //
    // FILE_NAME_END
    writer.Write(normalized_dict[FILENAME_END]);
    // FILE
    while (reader.Size()) {
        auto bitset_ch = reader.Read(ByteSize);
        writer.Write(normalized_dict[ConvertBitsetToEncodingType(bitset_ch)]);
    }
    if (is_next_file_exists) {
        writer.Write(normalized_dict[ONE_MORE_FILE]);
    } else {
        writer.Write(normalized_dict[ARCHIVE_END]);
        writer.Flush();
    }
}

void Encode(std::vector<const char*> input_files_names, const char *output_file_name) {
    std::ofstream out(output_file_name, std::ios::binary | std::ios::out);
    Writer writer(out);
    for (size_t index = 0; index < input_files_names.size(); ++index) {
        std::ifstream in(input_files_names[index], std::ios::binary | std::ios::in);
        Reader reader(in);
        EncodeFile(input_files_names[index], reader, writer, index + 1 != input_files_names.size());
    }
    out.close();
}