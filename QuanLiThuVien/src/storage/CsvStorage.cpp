#include "CsvStorage.h"

#include <filesystem>
#include <fstream>
#include <sstream>

namespace {
constexpr char kDelimiter = ';';

bool ensureParentFolder(const std::string& path, std::string& error) {
    std::filesystem::path filePath(path);
    std::filesystem::path parent = filePath.parent_path();
    if (parent.empty()) {
        return true;
    }

    std::error_code ec;
    std::filesystem::create_directories(parent, ec);
    if (ec) {
        error = "Failed to create directory: " + parent.string();
        return false;
    }
    return true;
}
}  // namespace

CsvStorage::CsvStorage(std::string booksPath, std::string readersPath, std::string recordsPath)
    : booksPath_(std::move(booksPath)),
      readersPath_(std::move(readersPath)),
      recordsPath_(std::move(recordsPath)) {}

bool CsvStorage::loadBooks(std::vector<Book>& books, std::string& error) {
    books.clear();
    std::ifstream file(booksPath_);
    if (!file.is_open()) {
        return true;
    }

    std::string line;
    bool isFirstLine = true;
    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }
        if (isFirstLine) {
            isFirstLine = false;
            if (line == "id;title;author;totalQuantity;availableQuantity") {
                continue;
            }
        }
        auto cols = split(line, kDelimiter);
        if (cols.size() != 5) {
            error = "Invalid books.csv format.";
            return false;
        }
        Book book;
        book.id = trim(cols[0]);
        book.title = trim(cols[1]);
        book.author = trim(cols[2]);
        try {
            book.totalQuantity = std::stoi(trim(cols[3]));
            book.availableQuantity = std::stoi(trim(cols[4]));
        } catch (...) {
            error = "Invalid number in books.csv.";
            return false;
        }
        books.push_back(book);
    }
    return true;
}

bool CsvStorage::saveBooks(const std::vector<Book>& books, std::string& error) {
    if (!ensureParentFolder(booksPath_, error)) {
        return false;
    }

    std::ofstream file(booksPath_, std::ios::trunc);
    if (!file.is_open()) {
        error = "Cannot open books file for writing.";
        return false;
    }
    file << "id;title;author;totalQuantity;availableQuantity\n";
    for (const auto& book : books) {
        file << book.id << kDelimiter << book.title << kDelimiter << book.author << kDelimiter
             << book.totalQuantity << kDelimiter << book.availableQuantity << "\n";
    }
    return true;
}

bool CsvStorage::loadReaders(std::vector<Reader>& readers, std::string& error) {
    readers.clear();
    std::ifstream file(readersPath_);
    if (!file.is_open()) {
        return true;
    }

    std::string line;
    bool isFirstLine = true;
    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }
        if (isFirstLine) {
            isFirstLine = false;
            if (line == "id;name;phone") {
                continue;
            }
        }
        auto cols = split(line, kDelimiter);
        if (cols.size() != 3) {
            error = "Invalid readers.csv format.";
            return false;
        }
        Reader reader;
        reader.id = trim(cols[0]);
        reader.name = trim(cols[1]);
        reader.phone = trim(cols[2]);
        readers.push_back(reader);
    }
    return true;
}

bool CsvStorage::saveReaders(const std::vector<Reader>& readers, std::string& error) {
    if (!ensureParentFolder(readersPath_, error)) {
        return false;
    }

    std::ofstream file(readersPath_, std::ios::trunc);
    if (!file.is_open()) {
        error = "Cannot open readers file for writing.";
        return false;
    }
    file << "id;name;phone\n";
    for (const auto& reader : readers) {
        file << reader.id << kDelimiter << reader.name << kDelimiter << reader.phone << "\n";
    }
    return true;
}

bool CsvStorage::loadBorrowRecords(std::vector<BorrowRecord>& records, std::string& error) {
    records.clear();
    std::ifstream file(recordsPath_);
    if (!file.is_open()) {
        return true;
    }

    std::string line;
    bool isFirstLine = true;
    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }
        if (isFirstLine) {
            isFirstLine = false;
            if (line == "id;readerId;bookId;borrowDate;returnDate;isReturned") {
                continue;
            }
        }
        auto cols = split(line, kDelimiter);
        if (cols.size() != 6) {
            error = "Invalid borrow_records.csv format.";
            return false;
        }
        BorrowRecord record;
        record.id = trim(cols[0]);
        record.readerId = trim(cols[1]);
        record.bookId = trim(cols[2]);
        record.borrowDate = trim(cols[3]);
        record.returnDate = trim(cols[4]);
        record.isReturned = trim(cols[5]) == "1";
        records.push_back(record);
    }
    return true;
}

bool CsvStorage::saveBorrowRecords(const std::vector<BorrowRecord>& records, std::string& error) {
    if (!ensureParentFolder(recordsPath_, error)) {
        return false;
    }

    std::ofstream file(recordsPath_, std::ios::trunc);
    if (!file.is_open()) {
        error = "Cannot open records file for writing.";
        return false;
    }
    file << "id;readerId;bookId;borrowDate;returnDate;isReturned\n";
    for (const auto& record : records) {
        file << record.id << kDelimiter << record.readerId << kDelimiter << record.bookId << kDelimiter
             << record.borrowDate << kDelimiter << record.returnDate << kDelimiter
             << (record.isReturned ? "1" : "0") << "\n";
    }
    return true;
}

std::vector<std::string> CsvStorage::split(const std::string& line, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    if (!line.empty() && line.back() == delimiter) {
        tokens.emplace_back();
    }
    return tokens;
}

std::string CsvStorage::trim(const std::string& value) {
    const auto start = value.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
        return "";
    }
    const auto end = value.find_last_not_of(" \t\r\n");
    return value.substr(start, end - start + 1);
}
