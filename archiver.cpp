#include <fstream>
#include <sstream>

#include "encode.h"
#include "decode.h"
#include "exception.h"
#include "parser.h"

template <typename T1, typename T2>
std::unordered_map<T2, T1> ReverseMap(std::unordered_map<T1, T2> &map) {
    std::unordered_map<T2, T1> reversed_map;
    for (auto &[a, b] : map) {
        reversed_map[b] = a;
    }
    return reversed_map;
}

constexpr const int ErrorCode = 111;

enum class ArgumentsType { ARCHIVATE = 0, UNARCHIVATE = 1, HELP };

int main(int argc, char **argv) {

    std::unordered_map<ArgumentsType, std::string> argument_type_to_string = {
        {ArgumentsType::ARCHIVATE, "-c"}, {ArgumentsType::UNARCHIVATE, "-d"}, {ArgumentsType::HELP, "-h"}};
    std::unordered_map<std::string, ArgumentsType> string_to_argument_type = ReverseMap(argument_type_to_string);

    try {
        auto parser_result =
            Parse(argc, argv,
                  {{argument_type_to_string[ArgumentsType::ARCHIVATE], KeyWithArgumentsType::UNKNOWN_COUNT_OF_VALUES},
                   {argument_type_to_string[ArgumentsType::UNARCHIVATE], KeyWithArgumentsType::ONE_VALUE},
                   {argument_type_to_string[ArgumentsType::HELP], KeyWithArgumentsType::WITHOUT_VALUE}});

        if (parser_result.key_words_with_value.empty()) {
            throw ParserException("No arguments were provided.");
        }
        if (parser_result.key_words_with_value.size() > 1) {
            throw ParserException("Too many arguments were provided.");
        }

        std::string argument_type = parser_result.key_words_with_value.begin()->first;

        switch (string_to_argument_type[argument_type]) {
            case ArgumentsType::ARCHIVATE: {
                std::string archive_name = parser_result.key_words_with_value[argument_type][0];
                std::vector<std::string> file_names;

                std::string path = "/opt/shad/tasks/archiver/tests/data/compress_decompress/";

                for (size_t index = 1; index < parser_result.key_words_with_value[argument_type].size(); ++index) {
                    file_names.push_back(parser_result.key_words_with_value[argument_type][index]);
                }

                Encode(path, file_names, archive_name);
                break;
            }
            case ArgumentsType::UNARCHIVATE: {
                std::string archive_name = parser_result.key_words_with_value[argument_type][0];
                Decode(archive_name);
                break;
            }
            case ArgumentsType::HELP: {
                std::cout << "Commands available:\n"
                             "-h Help\n"
                             "-c archive_name file1 [file2 ...] | archiving files in archive_name\n"
                             "-d archive_name | unarchiving files into this directory\n";
                break;
            }
        }
    } catch (std::exception &exception) {
        std::cerr << exception.what() << std::endl;
        return ErrorCode;
    }
}