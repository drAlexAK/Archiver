#include "trie.h"

TrieNode::TrieNode()
    : ch_(0), min_in_subtree_(EncodingTypeMax), quantity_(0), is_terminated_(false), left_(nullptr), right_(nullptr) {
}

TrieNode::TrieNode(EncodingType ch, size_t quantity, bool is_terminated, EncodingType min_in_subtree)
    : ch_(ch),
      min_in_subtree_(min_in_subtree),
      quantity_(quantity),
      is_terminated_(is_terminated),
      left_(nullptr),
      right_(nullptr) {
}

TrieNode::TrieNode(EncodingType ch, TrieNode *&left, TrieNode *&right, size_t quantity, bool is_terminated,
                   EncodingType min_in_subtree)
    : ch_(ch),
      min_in_subtree_(min_in_subtree),
      quantity_(quantity),
      is_terminated_(is_terminated),
      left_(left),
      right_(right) {
}

bool operator<(const TrieNode &a, const TrieNode &b) {
    return std::make_pair(a.quantity_, a.min_in_subtree_) < std::make_pair(b.quantity_, b.min_in_subtree_);
}

void Trie::PathBuild(TrieNode *&node, DBitset &path, size_t size, EncodingType ch) {
    if (size == path.size()) {
        node->is_terminated_ = true;
        return;
    }
    bool bit = path[size];
    if (bit) {
        if (!node->right_) {
            node->right_ = new TrieNode(ch, 1, false, ch);
        }
        PathBuild(node->right_, path, size + 1, ch);
    } else {
        if (!node->left_) {
            node->left_ = new TrieNode(ch, 1, false, ch);
        }
        PathBuild(node->left_, path, size + 1, ch);
    }
}

void Trie::Dfs(TrieNode *&node) {
    if (!node) {
        return;
    }
    if (node->left_) {
        Dfs(node->left_);
    }
    if (node->right_) {
        Dfs(node->right_);
    }
    delete node;
};

bool Trie::Cmp(TrieNode *a, TrieNode *b) {
    return *a < *b;
}

TrieNode *Trie::BuildTrieByFrequences(std::unordered_map<EncodingType, int> &byte_frequency) {
    Heap<TrieNode *> heap(byte_frequency.size(), Trie::Cmp);
    for (auto &[byte, count] : byte_frequency) {
        heap.Push(new TrieNode(byte, count, true, byte));
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

TrieNode *Trie::Merge(TrieNode *&left, TrieNode *&right) {
    TrieNode *head = new TrieNode(0, left, right, left->quantity_ + right->quantity_, false,
                                  std::min(left->min_in_subtree_, right->min_in_subtree_));
    return head;
}

std::unordered_map<EncodingType, boost::dynamic_bitset<>> Trie::GetEncodingTable(TrieNode *&head) {
    std::unordered_map<EncodingType, boost::dynamic_bitset<>> result;
    auto dfs = [&](auto self, TrieNode *&node, boost::dynamic_bitset<> &path, int bit) -> void {
        if (node->is_terminated_) {
            result[node->ch_] = path;
            return;
        }
        if (node->left_) {
            path.push_back(false);
            self(self, node->left_, path, bit + 1);
            path.pop_back();
        }
        if (node->right_) {
            path.push_back(true);
            self(self, node->right_, path, bit + 1);
            path.pop_back();
        }
    };
    boost::dynamic_bitset<> path;
    dfs(dfs, head, path, 0);
    return result;
}

void Trie::Clear(TrieNode *&head) {
    Dfs(head);
}

TrieNode *Trie::BuildTrieByPaths(std::unordered_map<EncodingType, DBitset> &map) {
    TrieNode *head = new TrieNode();
    for (auto [ch, path] : map) {
        PathBuild(head, path, 0, ch);
    }
    return head;
}