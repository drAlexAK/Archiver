#pragma once

#include <fstream>
#include <iostream>
#include <cstdint>

#include "encoderType.h"
#include "exception.h"

#include <boost/dynamic_bitset.hpp>

#define ByteSize 8

class Writer {
    std::ofstream &out_;
    EncodingType buffer_ = 0;
    int buffer_size_ = 0;

    void Write();

public:
    explicit Writer(std::ofstream &out);

    void Write(DBitset val);

    void Flush();
};

#undef ByteSize