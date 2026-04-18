#pragma once

#include <string>

#include "IStorage.h"

class CsvStorage : public IStorage {
public:
    CsvStorage(std::string booksPath, std::string readersPath, std::string recordsPath);

    bool loadBooks(std::vector<Book>& books, std::string& error) override;
    bool saveBooks(const std::vector<Book>& books, std::string& error) override;

    bool loadReaders(std::vector<Reader>& readers, std::string& error) override;
    bool saveReaders(const std::vector<Reader>& readers, std::string& error) override;

    bool loadBorrowRecords(std::vector<BorrowRecord>& records, std::string& error) override;
    bool saveBorrowRecords(const std::vector<BorrowRecord>& records, std::string& error) override;

private:
    std::string booksPath_;
    std::string readersPath_;
    std::string recordsPath_;

    static std::vector<std::string> split(const std::string& line, char delimiter);
    static std::string trim(const std::string& value);
};
