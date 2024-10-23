#include "writer.h"

void Writer::Write() {
    if (buffer_size_ == 0) {
        return;
    }
    if (!out_.put(static_cast<char>(buffer_ & ((1 << ByteSize) - 1)))) {
        throw WriterException("Trying to write in closed file.");
    }
    buffer_ = 0;
    buffer_size_ = 0;
}

Writer::Writer(std::ofstream &out) : out_(out) {
}

void Writer::Write(DBitset val) {
    for (size_t i = 0; i < val.size(); ++i) {
        buffer_ |= val[i] << buffer_size_;
        buffer_size_++;
        if (buffer_size_ == ByteSize) {
            Flush();
        }
    }
}

void Writer::Flush() {
    Write();
}