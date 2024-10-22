#pragma once

#include "encoderType.h"
#include "heap.h"

#include <functional>
#include <cstdint>

#include <boost/dynamic_bitset.hpp>

struct TrieNode {
private:
    EncodingType ch_ = 0;
    EncodingType min_in_subtree_ = EncodingTypeMax;
    size_t quantity_ = 0;
    bool is_terminated_ = false;

public:
    TrieNode* left_ = nullptr;
    TrieNode* right_ = nullptr;
    TrieNode()
        : ch_(0),
          min_in_subtree_(EncodingTypeMax),
          quantity_(0),
          is_terminated_(false),
          left_(nullptr),
          right_(nullptr) {
    }
    TrieNode(EncodingType ch, TrieNode* left, TrieNode* right, size_t quantity, bool is_terminated,
                EncodingType min_in_subtree)
        : ch_(ch),
          min_in_subtree_(min_in_subtree),
          quantity_(quantity),
          is_terminated_(is_terminated),
          left_(left),
          right_(right) {
    }

    EncodingType GetChar() const {
        return ch_;
    }
    EncodingType GetMinInSubtree() const {
        return min_in_subtree_;
    }
    TrieNode* GetLeft() const {
        return left_;
    }
    TrieNode* GetRight() const {
        return right_;
    }
    size_t GetQuantity() const {
        return quantity_;
    }
    void SetTerminated(bool bl) {
        is_terminated_ = bl;
    }
    bool IsTerminated() const {
        return is_terminated_;
    }
};
bool operator<(const TrieNode& a, const TrieNode& b) {
    return std::make_pair(a.GetQuantity(), a.GetMinInSubtree()) < std::make_pair(b.GetQuantity(), b.GetMinInSubtree());
}

class Trie {
    static TrieNode* PathBuild(TrieNode* node, DBitset path, size_t size, EncodingType ch) {
        if (!node) {
            node = new TrieNode(ch, nullptr, nullptr, 1, false, ch);
        }
        if (size == path.size()) {
            node->SetTerminated(true);
            return node;
        }
        bool bit = path[size];
        if (bit) {
            node->right_ = PathBuild(node->GetRight(), path, size + 1, ch);
        } else {
            node->left_ = PathBuild(node->GetLeft(), path, size + 1, ch);
        }
        return node;
    }

public:
    static bool Cmp(TrieNode* a, TrieNode* b) {
        return *a < *b;
    }

    static TrieNode* BuildTrieByFrequences(std::unordered_map<EncodingType, int> byte_frequency) {
        Heap<TrieNode*> heap(byte_frequency.size(), Trie::Cmp);
        for (auto& [byte, count] : byte_frequency) {
            heap.Push(new TrieNode(byte, nullptr, nullptr, count, true, byte));
        }

        while (heap.Size() > 1) {
            auto first = heap.Top();
            heap.Pop();
            auto second = heap.Top();
            heap.Pop();
            heap.Push(Trie::Merge(first, second));
        }
        return heap.Top();
    }

    static TrieNode* Merge(TrieNode* left, TrieNode* right) {
        TrieNode* head = new TrieNode(0, left, right, left->GetQuantity() + right->GetQuantity(), false,
                                            std::min(left->GetMinInSubtree(), right->GetMinInSubtree()));
        return head;
    }
    static std::unordered_map<EncodingType, boost::dynamic_bitset<>> GetEncodingTable(TrieNode* head) {
        std::unordered_map<EncodingType, boost::dynamic_bitset<>> result;
        auto dfs = [&](auto self, TrieNode* node, boost::dynamic_bitset<>& path, int bit) -> void {
            if (node->IsTerminated()) {
                result[node->GetChar()] = path;
                return;
            }
            if (node->GetLeft()) {
                path.push_back(false);
                self(self, node->GetLeft(), path, bit + 1);
                path.pop_back();
            }
            if (node->GetRight()) {
                path.push_back(true);
                self(self, node->GetRight(), path, bit + 1);
                path.pop_back();
            }
        };
        boost::dynamic_bitset<> path;
        dfs(dfs, head, path, 0);
        return result;
    }
    static void Clear(TrieNode* head) {
        auto dfs = [&](auto self, TrieNode* node) -> void {
            if (node->GetLeft()) {
                self(self, node->GetLeft());
            }
            if (node->GetRight()) {
                self(self, node->GetRight());
            }
            delete node;
        };
        dfs(dfs, head);
    }
    static TrieNode* BuildTrieByPaths(std::unordered_map<EncodingType, DBitset>& map) {
        TrieNode* head = new TrieNode();
        for (auto [ch, path] : map) {
            head = PathBuild(head, path, 0, ch);
        }
        return head;
    }
};