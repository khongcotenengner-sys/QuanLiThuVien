#pragma once

#include <string>

#include "../core/Library.h"

class Menu {
public:
    explicit Menu(Library& library);

    void run();

private:
    Library& library_;

    void showMainMenu() const;
    void handleBooks();
    void handleReaders();
    void handleBorrowBook();
    void handleReturnBook();
    void handleListBorrowRecords() const;

    void addBook();
    void updateBook();
    void removeBook();
    void listBooks() const;
    void searchBooks() const;

    void addReader();
    void updateReader();
    void removeReader();
    void listReaders() const;
    void searchReaders() const;

    static std::string inputLine(const std::string& prompt);
    static int inputInt(const std::string& prompt, int defaultValue = 0);
};
