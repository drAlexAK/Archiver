#pragma once
#include <vector>
#include <cstddef>
#include <assert.h>
#include <functional>

template <typename T>
class Heap {
    std::function<bool(T, T)> cmp_;
    std::vector<T> tree_;
    size_t size_;
    size_t GetLeftIndex(size_t index) {
        if ((index << 1) <= size_) {
            return index << 1;
        }
        return 0;
    }
    size_t GetRightIndex(size_t index) {
        if ((index << 1 | 1) <= size_) {
            return index << 1 | 1;
        }
        return 0;
    }
    void PushUp(size_t index) {
        while (index > 1) {
            if (cmp_(tree_[index], tree_[index >> 1])) {
                std::swap(tree_[index >> 1], tree_[index]);
            } else {
                break;
            }
            index >>= 1;
        }
    }
    void PushDown(size_t index) {
        while (GetLeftIndex(index) || GetRightIndex(index)) {
            int left = GetLeftIndex(index);
            int right = GetRightIndex(index);
            if (left && right) {
                if (cmp_(tree_[left], tree_[right])) {
                    std::swap(left, right);
                }
            }
            if (right && cmp_(tree_[right], tree_[index])) {
                std::swap(tree_[right], tree_[index]);
                index = right;
            } else if (left && cmp_(tree_[left], tree_[index])) {
                std::swap(tree_[left], tree_[index]);
                index = left;
            } else {
                break;
            }
        }
    }

public:
    explicit Heap(auto fun) {
        cmp_ = fun;
        size_ = 0;
        tree_.push_back(T());
    }
    explicit Heap(size_t size, auto fun) {
        cmp_ = fun;
        size_ = 0;
        tree_.reserve(size + 1);
        tree_.push_back(T());
    }
    ~Heap() {
        Clear();
    }
    void Push(const T& val) {
        tree_.push_back(val);
        size_++;
        PushUp(size_);
    }
    void Pop() {
        std::swap(tree_[1], tree_[size_]);
        tree_.pop_back();
        size_--;
        PushDown(1);
    }
    T Top() const {
        if (Empty()) {
            // TODO
            assert(false);
            // throw exception
        }
        return tree_[1];
    }
    bool Empty() const {
        return size_ == 0;
    }
    size_t Size() const {
        return size_;
    }
    void Clear() {
        tree_.clear();
        size_ = 0;
    }
};