#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../models/Book.h"
#include "../models/BorrowRecord.h"
#include "../models/Reader.h"
#include "../storage/IStorage.h"

class Library {
public:
    explicit Library(std::unique_ptr<IStorage> storage);

    bool loadData(std::string& error);
    bool saveData(std::string& error);

    bool addBook(const Book& book, std::string& error);
    bool updateBook(const Book& book, std::string& error);
    bool removeBook(const std::string& bookId, std::string& error);
    std::vector<Book> searchBooks(const std::string& keyword) const;

    bool addReader(const Reader& reader, std::string& error);
    bool updateReader(const Reader& reader, std::string& error);
    bool removeReader(const std::string& readerId, std::string& error);
    std::vector<Reader> searchReaders(const std::string& keyword) const;

    bool borrowBook(
        const std::string& recordId,
        const std::string& readerId,
        const std::string& bookId,
        const std::string& borrowDate,
        std::string& error
    );
    bool returnBook(const std::string& recordId, const std::string& returnDate, std::string& error);

    const std::vector<Book>& getBooks() const;
    const std::vector<Reader>& getReaders() const;
    const std::vector<BorrowRecord>& getBorrowRecords() const;

private:
    static constexpr int kMaxBorrowPerReader = 3;

    std::vector<Book> books_;
    std::vector<Reader> readers_;
    std::vector<BorrowRecord> records_;
    std::unique_ptr<IStorage> storage_;

    static std::string toLower(std::string input);

    Book* findBookById(const std::string& bookId);
    const Book* findBookById(const std::string& bookId) const;

    Reader* findReaderById(const std::string& readerId);
    const Reader* findReaderById(const std::string& readerId) const;

    BorrowRecord* findRecordById(const std::string& recordId);
    const BorrowRecord* findRecordById(const std::string& recordId) const;
};
