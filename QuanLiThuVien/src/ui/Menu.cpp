#include "Menu.h"

#include <algorithm>
#include <iostream>

Menu::Menu(Library& library) : library_(library) {}

void Menu::run() {
    while (true) {
        showMainMenu();
        const int choice = inputInt("Chon chuc nang: ");
        switch (choice) {
            case 1:
                handleBooks();
                break;
            case 2:
                handleReaders();
                break;
            case 3:
                handleBorrowBook();
                break;
            case 4:
                handleReturnBook();
                break;
            case 5:
                handleListBorrowRecords();
                break;
            case 0: {
                std::string error;
                if (!library_.saveData(error)) {
                    std::cout << "Luu du lieu that bai: " << error << "\n";
                } else {
                    std::cout << "Da luu du lieu.\n";
                }
                return;
            }
            default:
                std::cout << "Lua chon khong hop le.\n";
                break;
        }
    }
}

void Menu::showMainMenu() const {
    std::cout << "\n===== QUAN LI THU VIEN =====\n";
    std::cout << "1. Quan li sach\n";
    std::cout << "2. Quan li doc gia\n";
    std::cout << "3. Muon sach\n";
    std::cout << "4. Tra sach\n";
    std::cout << "5. Danh sach phieu muon\n";
    std::cout << "0. Luu va thoat\n";
}

void Menu::handleBooks() {
    std::cout << "\n--- QUAN LI SACH ---\n";
    std::cout << "1. Them sach\n";
    std::cout << "2. Sua sach\n";
    std::cout << "3. Xoa sach\n";
    std::cout << "4. Danh sach sach\n";
    std::cout << "5. Tim sach\n";
    int choice = inputInt("Chon: ");
    switch (choice) {
        case 1:
            addBook();
            break;
        case 2:
            updateBook();
            break;
        case 3:
            removeBook();
            break;
        case 4:
            listBooks();
            break;
        case 5:
            searchBooks();
            break;
        default:
            std::cout << "Lua chon khong hop le.\n";
            break;
    }
}

void Menu::handleReaders() {
    std::cout << "\n--- QUAN LI DOC GIA ---\n";
    std::cout << "1. Them doc gia\n";
    std::cout << "2. Sua doc gia\n";
    std::cout << "3. Xoa doc gia\n";
    std::cout << "4. Danh sach doc gia\n";
    std::cout << "5. Tim doc gia\n";
    int choice = inputInt("Chon: ");
    switch (choice) {
        case 1:
            addReader();
            break;
        case 2:
            updateReader();
            break;
        case 3:
            removeReader();
            break;
        case 4:
            listReaders();
            break;
        case 5:
            searchReaders();
            break;
        default:
            std::cout << "Lua chon khong hop le.\n";
            break;
    }
}

void Menu::handleBorrowBook() {
    std::string recordId = inputLine("Nhap ma phieu muon: ");
    std::string readerId = inputLine("Nhap ma doc gia: ");
    std::string bookId = inputLine("Nhap ma sach: ");
    std::string borrowDate = inputLine("Nhap ngay muon (YYYY-MM-DD): ");

    std::string error;
    if (library_.borrowBook(recordId, readerId, bookId, borrowDate, error)) {
        std::cout << "Muon sach thanh cong.\n";
    } else {
        std::cout << "Muon sach that bai: " << error << "\n";
    }
}

void Menu::handleReturnBook() {
    std::string recordId = inputLine("Nhap ma phieu muon: ");
    std::string returnDate = inputLine("Nhap ngay tra (YYYY-MM-DD): ");

    std::string error;
    if (library_.returnBook(recordId, returnDate, error)) {
        std::cout << "Tra sach thanh cong.\n";
    } else {
        std::cout << "Tra sach that bai: " << error << "\n";
    }
}

void Menu::handleListBorrowRecords() const {
    std::cout << "\n--- DANH SACH PHIEU MUON ---\n";
    for (const auto& record : library_.getBorrowRecords()) {
        std::cout << "Ma phieu: " << record.id << " | Doc gia: " << record.readerId
                  << " | Sach: " << record.bookId << " | Ngay muon: " << record.borrowDate
                  << " | Trang thai: " << (record.isReturned ? "Da tra" : "Dang muon");
        if (record.isReturned) {
            std::cout << " | Ngay tra: " << record.returnDate;
        }
        std::cout << "\n";
    }
}

void Menu::addBook() {
    Book book;
    book.id = inputLine("Ma sach: ");
    book.title = inputLine("Ten sach: ");
    book.author = inputLine("Tac gia: ");
    book.totalQuantity = inputInt("Tong so luong: ");
    book.availableQuantity = book.totalQuantity;

    std::string error;
    if (library_.addBook(book, error)) {
        std::cout << "Them sach thanh cong.\n";
    } else {
        std::cout << "Them sach that bai: " << error << "\n";
    }
}

void Menu::updateBook() {
    std::string id = inputLine("Nhap ma sach can sua: ");
    const auto& books = library_.getBooks();
    auto it = std::find_if(books.begin(), books.end(), [&](const Book& b) { return b.id == id; });
    if (it == books.end()) {
        std::cout << "Khong tim thay sach.\n";
        return;
    }

    Book updated = *it;
    updated.title = inputLine("Ten sach moi: ");
    updated.author = inputLine("Tac gia moi: ");
    updated.totalQuantity = inputInt("Tong so luong moi: ", updated.totalQuantity);
    int borrowedCount = it->totalQuantity - it->availableQuantity;
    updated.availableQuantity = updated.totalQuantity - borrowedCount;

    std::string error;
    if (library_.updateBook(updated, error)) {
        std::cout << "Cap nhat sach thanh cong.\n";
    } else {
        std::cout << "Cap nhat sach that bai: " << error << "\n";
    }
}

void Menu::removeBook() {
    std::string id = inputLine("Nhap ma sach can xoa: ");
    std::string error;
    if (library_.removeBook(id, error)) {
        std::cout << "Xoa sach thanh cong.\n";
    } else {
        std::cout << "Xoa sach that bai: " << error << "\n";
    }
}

void Menu::listBooks() const {
    std::cout << "\n--- DANH SACH SACH ---\n";
    for (const auto& book : library_.getBooks()) {
        std::cout << "Ma: " << book.id << " | Ten: " << book.title << " | Tac gia: " << book.author
                  << " | Tong: " << book.totalQuantity << " | Con: " << book.availableQuantity << "\n";
    }
}

void Menu::searchBooks() const {
    std::string keyword = inputLine("Nhap tu khoa tim sach: ");
    auto result = library_.searchBooks(keyword);
    std::cout << "\nKet qua tim sach:\n";
    for (const auto& book : result) {
        std::cout << "Ma: " << book.id << " | Ten: " << book.title << " | Tac gia: " << book.author
                  << " | Tong: " << book.totalQuantity << " | Con: " << book.availableQuantity << "\n";
    }
}

void Menu::addReader() {
    Reader reader;
    reader.id = inputLine("Ma doc gia: ");
    reader.name = inputLine("Ten doc gia: ");
    reader.phone = inputLine("So dien thoai: ");

    std::string error;
    if (library_.addReader(reader, error)) {
        std::cout << "Them doc gia thanh cong.\n";
    } else {
        std::cout << "Them doc gia that bai: " << error << "\n";
    }
}

void Menu::updateReader() {
    std::string id = inputLine("Nhap ma doc gia can sua: ");
    const auto& readers = library_.getReaders();
    auto it = std::find_if(readers.begin(), readers.end(), [&](const Reader& r) { return r.id == id; });
    if (it == readers.end()) {
        std::cout << "Khong tim thay doc gia.\n";
        return;
    }

    Reader updated = *it;
    updated.name = inputLine("Ten moi: ");
    updated.phone = inputLine("So dien thoai moi: ");

    std::string error;
    if (library_.updateReader(updated, error)) {
        std::cout << "Cap nhat doc gia thanh cong.\n";
    } else {
        std::cout << "Cap nhat doc gia that bai: " << error << "\n";
    }
}

void Menu::removeReader() {
    std::string id = inputLine("Nhap ma doc gia can xoa: ");
    std::string error;
    if (library_.removeReader(id, error)) {
        std::cout << "Xoa doc gia thanh cong.\n";
    } else {
        std::cout << "Xoa doc gia that bai: " << error << "\n";
    }
}

void Menu::listReaders() const {
    std::cout << "\n--- DANH SACH DOC GIA ---\n";
    for (const auto& reader : library_.getReaders()) {
        std::cout << "Ma: " << reader.id << " | Ten: " << reader.name << " | SDT: " << reader.phone << "\n";
    }
}

void Menu::searchReaders() const {
    std::string keyword = inputLine("Nhap tu khoa tim doc gia: ");
    auto result = library_.searchReaders(keyword);
    std::cout << "\nKet qua tim doc gia:\n";
    for (const auto& reader : result) {
        std::cout << "Ma: " << reader.id << " | Ten: " << reader.name << " | SDT: " << reader.phone << "\n";
    }
}

std::string Menu::inputLine(const std::string& prompt) {
    std::cout << prompt;
    std::string value;
    std::getline(std::cin, value);
    return value;
}

int Menu::inputInt(const std::string& prompt, int defaultValue) {
    std::string raw = inputLine(prompt);
    if (raw.empty()) {
        return defaultValue;
    }
    try {
        return std::stoi(raw);
    } catch (...) {
        return defaultValue;
    }
}
