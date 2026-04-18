#pragma once

#include <string>

struct BorrowRecord {
    std::string id;
    std::string readerId;
    std::string bookId;
    std::string borrowDate;
    std::string returnDate;
    bool isReturned = false;
};
