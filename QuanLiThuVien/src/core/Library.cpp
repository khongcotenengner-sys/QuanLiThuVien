#include "Library.h"

#include <algorithm>
#include <cctype>

Library::Library(std::unique_ptr<IStorage> storage) : storage_(std::move(storage)) {}

bool Library::loadData(std::string& error) {
    if (!storage_) {
        error = "Storage is not initialized.";
        return false;
    }

    books_.clear();
    readers_.clear();
    records_.clear();

    return storage_->loadBooks(books_, error) &&
           storage_->loadReaders(readers_, error) &&
           storage_->loadBorrowRecords(records_, error);
}

bool Library::saveData(std::string& error) {
    if (!storage_) {
        error = "Storage is not initialized.";
        return false;
    }

    return storage_->saveBooks(books_, error) &&
           storage_->saveReaders(readers_, error) &&
           storage_->saveBorrowRecords(records_, error);
}

bool Library::addBook(const Book& book, std::string& error) {
    if (book.id.empty()) {
        error = "Book ID must not be empty.";
        return false;
    }
    if (findBookById(book.id) != nullptr) {
        error = "Book ID already exists.";
        return false;
    }
    if (book.totalQuantity < 0 || book.availableQuantity < 0 || book.availableQuantity > book.totalQuantity) {
        error = "Invalid book quantity.";
        return false;
    }
    books_.push_back(book);
    return true;
}

bool Library::updateBook(const Book& book, std::string& error) {
    Book* existing = findBookById(book.id);
    if (!existing) {
        error = "Book not found.";
        return false;
    }
    if (book.totalQuantity < 0 || book.availableQuantity < 0 || book.availableQuantity > book.totalQuantity) {
        error = "Invalid book quantity.";
        return false;
    }

    int borrowedCount = existing->totalQuantity - existing->availableQuantity;
    if (book.totalQuantity < borrowedCount) {
        error = "Total quantity cannot be less than number of books currently borrowed.";
        return false;
    }
    if (book.availableQuantity != (book.totalQuantity - borrowedCount)) {
        error = "Available quantity must match current borrow state.";
        return false;
    }

    *existing = book;
    return true;
}

bool Library::removeBook(const std::string& bookId, std::string& error) {
    auto hasActiveBorrow = std::any_of(records_.begin(), records_.end(), [&](const BorrowRecord& record) {
        return record.bookId == bookId && !record.isReturned;
    });
    if (hasActiveBorrow) {
        error = "Cannot remove book while active borrow records exist.";
        return false;
    }

    auto it = std::remove_if(books_.begin(), books_.end(), [&](const Book& book) {
        return book.id == bookId;
    });

    if (it == books_.end()) {
        error = "Book not found.";
        return false;
    }

    books_.erase(it, books_.end());
    return true;
}

std::vector<Book> Library::searchBooks(const std::string& keyword) const {
    const std::string key = toLower(keyword);
    std::vector<Book> result;
    for (const auto& book : books_) {
        std::string composite = toLower(book.id + " " + book.title + " " + book.author);
        if (key.empty() || composite.find(key) != std::string::npos) {
            result.push_back(book);
        }
    }
    return result;
}

bool Library::addReader(const Reader& reader, std::string& error) {
    if (reader.id.empty()) {
        error = "Reader ID must not be empty.";
        return false;
    }
    if (findReaderById(reader.id) != nullptr) {
        error = "Reader ID already exists.";
        return false;
    }
    readers_.push_back(reader);
    return true;
}

bool Library::updateReader(const Reader& reader, std::string& error) {
    Reader* existing = findReaderById(reader.id);
    if (!existing) {
        error = "Reader not found.";
        return false;
    }
    *existing = reader;
    return true;
}

bool Library::removeReader(const std::string& readerId, std::string& error) {
    auto hasActiveBorrow = std::any_of(records_.begin(), records_.end(), [&](const BorrowRecord& record) {
        return record.readerId == readerId && !record.isReturned;
    });
    if (hasActiveBorrow) {
        error = "Cannot remove reader while active borrow records exist.";
        return false;
    }

    auto it = std::remove_if(readers_.begin(), readers_.end(), [&](const Reader& reader) {
        return reader.id == readerId;
    });
    if (it == readers_.end()) {
        error = "Reader not found.";
        return false;
    }
    readers_.erase(it, readers_.end());
    return true;
}

std::vector<Reader> Library::searchReaders(const std::string& keyword) const {
    const std::string key = toLower(keyword);
    std::vector<Reader> result;
    for (const auto& reader : readers_) {
        std::string composite = toLower(reader.id + " " + reader.name + " " + reader.phone);
        if (key.empty() || composite.find(key) != std::string::npos) {
            result.push_back(reader);
        }
    }
    return result;
}

bool Library::borrowBook(
    const std::string& recordId,
    const std::string& readerId,
    const std::string& bookId,
    const std::string& borrowDate,
    std::string& error
) {
    if (recordId.empty()) {
        error = "Record ID must not be empty.";
        return false;
    }
    if (findRecordById(recordId) != nullptr) {
        error = "Record ID already exists.";
        return false;
    }

    Reader* reader = findReaderById(readerId);
    if (!reader) {
        error = "Reader not found.";
        return false;
    }
    (void)reader;

    Book* book = findBookById(bookId);
    if (!book) {
        error = "Book not found.";
        return false;
    }
    if (book->availableQuantity <= 0) {
        error = "Book is out of stock.";
        return false;
    }

    int activeBorrowCount = 0;
    for (const auto& record : records_) {
        if (record.readerId == readerId && !record.isReturned) {
            ++activeBorrowCount;
            if (record.bookId == bookId) {
                error = "Reader already borrowed this book and has not returned it.";
                return false;
            }
        }
    }
    if (activeBorrowCount >= kMaxBorrowPerReader) {
        error = "Reader reached maximum allowed active borrows.";
        return false;
    }

    BorrowRecord record;
    record.id = recordId;
    record.readerId = readerId;
    record.bookId = bookId;
    record.borrowDate = borrowDate;
    record.returnDate.clear();
    record.isReturned = false;
    records_.push_back(record);
    --book->availableQuantity;
    return true;
}

bool Library::returnBook(const std::string& recordId, const std::string& returnDate, std::string& error) {
    BorrowRecord* record = findRecordById(recordId);
    if (!record) {
        error = "Borrow record not found.";
        return false;
    }
    if (record->isReturned) {
        error = "This borrow record is already returned.";
        return false;
    }

    Book* book = findBookById(record->bookId);
    if (!book) {
        error = "Book linked to this record was not found.";
        return false;
    }
    if (book->availableQuantity >= book->totalQuantity) {
        error = "Cannot return because book stock is inconsistent.";
        return false;
    }

    record->isReturned = true;
    record->returnDate = returnDate;
    ++book->availableQuantity;
    return true;
}

const std::vector<Book>& Library::getBooks() const { return books_; }

const std::vector<Reader>& Library::getReaders() const { return readers_; }

const std::vector<BorrowRecord>& Library::getBorrowRecords() const { return records_; }

std::string Library::toLower(std::string input) {
    std::transform(input.begin(), input.end(), input.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return input;
}

Book* Library::findBookById(const std::string& bookId) {
    auto it = std::find_if(books_.begin(), books_.end(), [&](const Book& book) {
        return book.id == bookId;
    });
    return (it == books_.end()) ? nullptr : &(*it);
}

const Book* Library::findBookById(const std::string& bookId) const {
    auto it = std::find_if(books_.begin(), books_.end(), [&](const Book& book) {
        return book.id == bookId;
    });
    return (it == books_.end()) ? nullptr : &(*it);
}

Reader* Library::findReaderById(const std::string& readerId) {
    auto it = std::find_if(readers_.begin(), readers_.end(), [&](const Reader& reader) {
        return reader.id == readerId;
    });
    return (it == readers_.end()) ? nullptr : &(*it);
}

const Reader* Library::findReaderById(const std::string& readerId) const {
    auto it = std::find_if(readers_.begin(), readers_.end(), [&](const Reader& reader) {
        return reader.id == readerId;
    });
    return (it == readers_.end()) ? nullptr : &(*it);
}

BorrowRecord* Library::findRecordById(const std::string& recordId) {
    auto it = std::find_if(records_.begin(), records_.end(), [&](const BorrowRecord& record) {
        return record.id == recordId;
    });
    return (it == records_.end()) ? nullptr : &(*it);
}

const BorrowRecord* Library::findRecordById(const std::string& recordId) const {
    auto it = std::find_if(records_.begin(), records_.end(), [&](const BorrowRecord& record) {
        return record.id == recordId;
    });
    return (it == records_.end()) ? nullptr : &(*it);
}
