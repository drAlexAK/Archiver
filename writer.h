#pragma once

#include <fstream>
#include <iostream>
#include <cstdint>

#include "encoderType.h"

#include <boost/dynamic_bitset.hpp>

#define ByteSize 8

class Writer {
    std::ofstream& out_;
    EncodingType buffer_ = 0;
    int buffer_size_ = 0;
    void Write() {
        if (buffer_size_ == 0) {
            return;
        }
        out_.put(static_cast<char>(buffer_ & ((1 << ByteSize) - 1)));
        buffer_ = 0;
        buffer_size_ = 0;
    }
public:
    explicit Writer(std::ofstream& out) : out_(out) {
    }
    void Write(boost::dynamic_bitset<> val) {
        for (size_t i = 0; i < val.size(); ++i) {
            buffer_ |= val[i] << buffer_size_;
            buffer_size_++;
            if (buffer_size_ == ByteSize) {
                Flush();
            }
        }
    }
    void Flush() {
        Write();
    }
};

#undef ByteSize