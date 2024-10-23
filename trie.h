#pragma once

#include "encoderType.h"
#include "heap.h"

#include <functional>
#include <cstdint>

#include <boost/dynamic_bitset.hpp>

struct TrieNode {
public:
    EncodingType ch_ = 0;
    EncodingType min_in_subtree_ = EncodingTypeMax;
    size_t quantity_ = 0;
    bool is_terminated_ = false;
    TrieNode *left_ = nullptr;
    TrieNode *right_ = nullptr;

    TrieNode();

    TrieNode(EncodingType ch, size_t quantity, bool is_terminated, EncodingType min_in_subtree);

    TrieNode(EncodingType ch, TrieNode *&left, TrieNode *&right, size_t quantity, bool is_terminated,
             EncodingType min_in_subtree);
};

bool operator<(const TrieNode &a, const TrieNode &b);

class Trie {
    static void PathBuild(TrieNode *&node, DBitset &path, size_t size, EncodingType ch);

    static void Dfs(TrieNode *&node);

public:
    static bool Cmp(TrieNode *a, TrieNode *b);

    static TrieNode *BuildTrieByFrequences(std::unordered_map<EncodingType, int> &byte_frequency);

    static TrieNode *Merge(TrieNode *&left, TrieNode *&right);

    static std::unordered_map<EncodingType, boost::dynamic_bitset<>> GetEncodingTable(TrieNode *&head);

    static void Clear(TrieNode *&head);

    static TrieNode *BuildTrieByPaths(std::unordered_map<EncodingType, DBitset> &map);
};