#include "encode.h"

std::unordered_map<EncodingType, int> GetFrequencyOfBytes(const std::string_view &file_name, Reader &reader) {
    std::unordered_map<EncodingType, int> result;
    for (auto c : file_name) {
        ++result[c];
    }
    while (reader.Size()) {
        auto read = reader.Read(ByteSize);
        ++result[ConvertBitsetToEncodingType(read)];
    }
    result[FilenameEnd] = 1;
    result[OneMoreFile] = 1;
    result[ArchiveEnd] = 1;
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
        result_vec.push_back(
            make_pair(pairs_of_char_and_len[i].second, DBitset(pairs_of_char_and_len[i].first, reversed_code)));
        if (i + 1 < pairs_of_char_and_len.size()) {
            code = (code + 1) << (pairs_of_char_and_len[i + 1].first - pairs_of_char_and_len[i].first);
        }
    }
    return std::make_pair(result_dict, result_vec);
}

void EncodeFile(std::string &file_name, Reader &reader, Writer &writer, bool is_next_file_exists) {
    std::unordered_map<EncodingType, int> byte_frequency = GetFrequencyOfBytes(file_name, reader);
    reader.Refresh();

    TrieNode *head = Trie::BuildTrieByFrequences(byte_frequency);
    std::unordered_map<EncodingType, boost::dynamic_bitset<>> trie_dict = Trie::GetEncodingTable(head);
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
    writer.Write(normalized_dict[FilenameEnd]);
    // FILE
    while (reader.Size()) {
        auto bitset_ch = reader.Read(ByteSize);
        writer.Write(normalized_dict[ConvertBitsetToEncodingType(bitset_ch)]);
    }
    if (is_next_file_exists) {
        writer.Write(normalized_dict[OneMoreFile]);
    } else {
        writer.Write(normalized_dict[ArchiveEnd]);
        writer.Flush();
    }
}

void Encode(std::string &input_file_path, std::vector<std::string> &input_files_names, std::string &output_file_name) {
    std::ofstream out(output_file_name.c_str(), std::ios::binary | std::ios::out);
    Writer writer(out);
    for (size_t index = 0; index < input_files_names.size(); ++index) {
        std::ifstream in((input_file_path + input_files_names[index]).c_str(), std::ios::binary | std::ios::in);
        Reader reader(in);
        EncodeFile(input_files_names[index], reader, writer, index + 1 != input_files_names.size());
    }
    out.close();
}