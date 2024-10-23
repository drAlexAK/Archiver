
#pragma once

#include "reader.h"
#include "writer.h"
#include "trie.h"
#include "encoderType.h"
#include "exception.h"

std::unordered_map<EncodingType, DBitset> DecodeNormalizedForm(std::vector<EncodingType> &alph,
                                                               std::vector<size_t> &lens);

EncodingType GetNextSymbol(Reader &reader, TrieNode *&node, TrieNode *&head);

void Decode(std::string &file_name);