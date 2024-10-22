#pragma once

#include <fstream>
#include <cstddef>
#include <iostream>
#include <assert.h>

#include "encoderType.h"

#include <boost/dynamic_bitset.hpp>

#define ByteSize 8

class Reader {
    std::ifstream &in_;
    int64_t size_ = 0;
    int64_t current_byte_ = 0;
    EncodingType buffer_ = 0;
    int buffer_size_ = 0;
    char tmp_;
public:
    explicit Reader(std::ifstream &in) : in_(in), current_byte_(0), buffer_(0), buffer_size_(0) {
        in.seekg(0, in.end);
        size_ = in.tellg();
        in.seekg(0);
    }
    boost::dynamic_bitset<> Read(size_t count_of_bits) {
        boost::dynamic_bitset<> result(count_of_bits);
        size_t cur_pos = 0;
        while (count_of_bits > 0) {
            while (buffer_size_ > 0 && count_of_bits > 0) {
                result[cur_pos] |= (buffer_ & 1ll);
                ++cur_pos;
                --buffer_size_;
                --count_of_bits;
                buffer_ >>= 1;
            }
            if (count_of_bits == 0) {
                break;
            }
            if (current_byte_ == size_) {
                assert(false);
                // TODO throw exception
            }
            in_.get(tmp_);
            buffer_ = tmp_;
            ++current_byte_;
            buffer_size_ = ByteSize;
        }
        return result;
    }
    int64_t Size() const {
        return size_ - current_byte_;
    }
    void Refresh() {
        in_.seekg(0, in_.end);
        size_ = in_.tellg();
        in_.seekg(0);
        current_byte_ = 0;
        buffer_ = 0;
        buffer_size_ = 0;
    }
};

#undef ByteSize