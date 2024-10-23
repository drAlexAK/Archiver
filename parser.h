#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "exception.h"

struct ParserResult {
    std::vector<std::string> key_words_without_values;
    std::unordered_map<std::string, std::vector<std::string>> key_words_with_value;
};

enum class KeyWithArgumentsType { WITHOUT_VALUE = 0, ONE_VALUE = 1, UNKNOWN_COUNT_OF_VALUES = 2 };

ParserResult Parse(int argc, char **argv, std::unordered_map<std::string, KeyWithArgumentsType> is_key_with_arguments);