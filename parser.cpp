#include "parser.h"

ParserResult Parse(int argc, char **argv, std::unordered_map<std::string, KeyWithArgumentsType> is_key_with_arguments) {
    ParserResult result;
    for (int i = 1; i < argc; ++i) {
        if (is_key_with_arguments.find(argv[i]) == is_key_with_arguments.end()) {
            throw ParserException("unknown key: " + std::string(argv[i]));
        } else {
            switch (is_key_with_arguments[argv[i]]) {
                case KeyWithArgumentsType::WITHOUT_VALUE:
                    result.key_words_with_value[argv[i]] = {};
                    break;
                case KeyWithArgumentsType::ONE_VALUE:
                    if (i + 1 >= argc) {
                        throw ParserException("Key" + std::string(argv[i]) +
                                              " expects one argument, none was provided.");
                    }
                    result.key_words_with_value[argv[i]].push_back(argv[i + 1]);
                    ++i;
                    break;
                case KeyWithArgumentsType::UNKNOWN_COUNT_OF_VALUES:
                    int index = i + 1;
                    while (index < argc) {
                        if (is_key_with_arguments.find(argv[index]) != is_key_with_arguments.end()) {
                            break;
                        }
                        result.key_words_with_value[argv[i]].push_back(argv[index]);
                        ++index;
                    }
                    i = index - 1;
                    break;
            }
        }
    }
    return result;
}