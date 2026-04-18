#pragma once

#include <string>

struct Book {
    std::string id;
    std::string title;
    std::string author;
    int totalQuantity = 0;
    int availableQuantity = 0;
};
