#pragma once

#include "heap.h"
#include "trie.h"
#include "reader.h"
#include "writer.h"
#include "encoderType.h"
#include "exception.h"

std::unordered_map<EncodingType, int> GetFrequencyOfBytes(const std::string_view &file_name, Reader &reader);

std::pair<std::unordered_map<EncodingType, DBitset>, std::vector<std::pair<EncodingType, DBitset>>> Normalize(
    const std::unordered_map<EncodingType, DBitset> &dict);

void EncodeFile(std::string &file_name, Reader &reader, Writer &writer, bool is_next_file_exists);

void Encode(std::string &input_file_path, std::vector<std::string> &input_files_names, std::string &output_file_name);