#pragma once

#include <string>
#include <vector>

#include "../models/Book.h"
#include "../models/BorrowRecord.h"
#include "../models/Reader.h"

class IStorage {
public:
    virtual ~IStorage() = default;

    virtual bool loadBooks(std::vector<Book>& books, std::string& error) = 0;
    virtual bool saveBooks(const std::vector<Book>& books, std::string& error) = 0;

    virtual bool loadReaders(std::vector<Reader>& readers, std::string& error) = 0;
    virtual bool saveReaders(const std::vector<Reader>& readers, std::string& error) = 0;

    virtual bool loadBorrowRecords(std::vector<BorrowRecord>& records, std::string& error) = 0;
    virtual bool saveBorrowRecords(const std::vector<BorrowRecord>& records, std::string& error) = 0;
};
