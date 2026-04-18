#include <iostream>
#include <filesystem>
#include <memory>

#include "core/Library.h"
#include "storage/CsvStorage.h"
#include "ui/Menu.h"

int main() {
    namespace fs = std::filesystem;

    auto resolveDataPath = []() {
        const fs::path candidates[] = {
            fs::path("data"),
            fs::path("../data"),
            fs::path("../../data")
        };
        for (const auto& candidate : candidates) {
            if (fs::exists(candidate / "books.csv") &&
                fs::exists(candidate / "readers.csv") &&
                fs::exists(candidate / "borrow_records.csv")) {
                return candidate;
            }
        }
        return fs::path("data");
    };

    const fs::path dataDir = resolveDataPath();

    auto storage = std::make_unique<CsvStorage>(
        (dataDir / "books.csv").string(),
        (dataDir / "readers.csv").string(),
        (dataDir / "borrow_records.csv").string()
    );

    Library library(std::move(storage));
    std::string error;
    if (!library.loadData(error)) {
        std::cout << "Canh bao: khong the tai du lieu: " << error << "\n";
    }

    Menu menu(library);
    menu.run();
    return 0;
}
