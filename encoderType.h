#pragma once

#include <cstdint>
#include <cstddef>

#include <boost/dynamic_bitset.hpp>

using EncodingType = uint16_t;
constexpr const EncodingType EncodingTypeMax = UINT16_MAX;
constexpr const size_t SpecialByteSize = 9;
constexpr const size_t ByteSize = 8;
constexpr const EncodingType FilenameEnd = 256;
constexpr const EncodingType OneMoreFile = 257;
constexpr const EncodingType ArchiveEnd = 258;

using DBitset = boost::dynamic_bitset<>;

DBitset ConvertEncodingTypeToBitset(EncodingType a);

EncodingType ConvertBitsetToEncodingType(DBitset a);

uint64_t ReverseCode(uint64_t code, size_t len);