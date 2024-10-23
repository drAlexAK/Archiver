#include "encoderType.h"

DBitset ConvertEncodingTypeToBitset(EncodingType a) {
    DBitset result;
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