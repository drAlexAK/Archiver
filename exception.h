#pragma once

#include <stdexcept>
#include <string>

class ArchiverException : public std::exception {
public:
    const char *what() const noexcept override {
        return message_.c_str();
    }
    ArchiverException(const std::string &ex_name, const std::string &message) {
        message_ = ex_name + " with message " + message;
    }

protected:
    std::string message_;
};

struct ParserException : public ArchiverException {
public:
    explicit ParserException(const std::string &message) : ArchiverException("Parser", message) {
    }
};

struct WriterException : public ArchiverException {
public:
    explicit WriterException(const std::string &message) : ArchiverException("Writer", message) {
    }
};

struct ReaderException : public ArchiverException {
public:
    explicit ReaderException(const std::string &message) : ArchiverException("Reader", message) {
    }
};

struct EncodeException : public ArchiverException {
public:
    explicit EncodeException(const std::string &message) : ArchiverException("Encode", message) {
    }
};

struct DecodeException : public ArchiverException {
public:
    explicit DecodeException(const std::string &message) : ArchiverException("Decode", message) {
    }
};

struct HeapException : public ArchiverException {
public:
    explicit HeapException(const std::string &message) : ArchiverException("Heap", message) {
    }
};