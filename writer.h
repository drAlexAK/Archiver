#pragma once

#include <fstream>
#include <iostream>
#include <cstdint>

#include "encoderType.h"
#include "exception.h"

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
        if (!out_.put(static_cast<char>(buffer_ & ((1 << ByteSize) - 1)))){
            throw WriterException("Trying to write in closed file.");
        }
        buffer_ = 0;
        buffer_size_ = 0;
    }
public:
    explicit Writer(std::ofstream& out) : out_(out) {
    }
    void Write(DBitset val) {
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