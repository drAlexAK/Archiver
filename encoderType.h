#pragma once

#include <cstdint>
#include <cstddef>

#include <boost/dynamic_bitset.hpp>

using EncodingType = uint16_t;
constexpr const EncodingType EncodingTypeMax = UINT16_MAX;
constexpr const size_t SpecialByteSize = 9;
constexpr const size_t ByteSize = 8;
constexpr const EncodingType FILENAME_END = 256;
constexpr const EncodingType ONE_MORE_FILE = 257;
constexpr const EncodingType ARCHIVE_END = 258;

using DBitset = boost::dynamic_bitset<>;

DBitset ConvertEncodingTypeToBitset(EncodingType a) {
    boost::dynamic_bitset result;
    for (size_t i = 0; i < ByteSize; ++i) {
        result.push_back(a >> i & 1);
    }
    return result;
}
EncodingType ConvertBitsetToEncodingType(DBitset a) {
    EncodingType result = 0;
    for (size_t i = 0; i < a.size(); ++i) {
        result |= a[i] << i;
    }
    return result;
}

uint64_t ReverseCode(uint64_t code, size_t len) {
    for (size_t bit = 0; len - bit - 1 > bit; ++bit) {
        uint64_t higher = (code >> (len - bit - 1) & 1);
        uint64_t lower = (code >> bit & 1);
        code -= higher << (len - bit - 1);
        code -= lower << bit;
        code += lower << (len - bit - 1);
        code += higher << bit;
    }
    return code;
}