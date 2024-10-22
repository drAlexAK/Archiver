#include <fstream>
#include "encode.h"
#include "decode.h"
#include <sstream>

int main(int argc, char** argv) {
    if (argv[1][0] == '+') {
            std::vector<const char*> file_names = {"hello.txt", "hello2.txt"};
            const char* archive_name = "my_archive";
            Encode(file_names, archive_name);
    } else {
        Decode("my_archive");
    }
}
