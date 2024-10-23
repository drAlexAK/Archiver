#pragma once

#include <fstream>
#include <cstddef>
#include <iostream>

#include "encoderType.h"
#include "exception.h"

#include <boost/dynamic_bitset.hpp>

#define ByteSize 8

class Reader {
    std::ifstream &in_;
    int64_t size_ = 0;
    int64_t current_byte_ = 0;
    EncodingType buffer_ = 0;
    int buffer_size_ = 0;
    char tmp_ = 0;

public:
    explicit Reader(std::ifstream &in);

    boost::dynamic_bitset<> Read(size_t count_of_bits);

    int64_t Size() const;

    void Refresh();
};

#undef ByteSize