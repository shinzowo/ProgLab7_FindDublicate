#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <set>
#include <unordered_map>


namespace fs = std::filesystem;

// Функция для вычисления хэша файла с помощью MD5
std::string calculate_md5(const fs::path& file_path, size_t block_size) {
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << file_path << std::endl;
        return "";
    }

    

    char* buffer = new char[block_size];
    size_t bytes_read = 0;

    while (file.read(buffer, block_size)) {
        bytes_read = file.gcount();
        
    }

    // Дополняем файл нулями, если его размер не кратен block_size
    if (file.gcount() > 0) {
        
    }

    

    // Преобразуем хэш в строку
    std::string hash_str;
    

    delete[] buffer;
    return hash_str;
}

// Функция для обхода директорий и поиска дубликатов
void scan_directory(const std::vector<std::string>& dirs, const std::vector<std::string>& exclude_dirs, 
                    size_t min_size, const std::string& file_mask, size_t block_size) {

    std::unordered_map<std::string, std::set<fs::path>> file_hash_map;

    // Обходим все директории
    for (const auto& dir : dirs) {
        fs::path directory(dir);
        if (!fs::exists(directory) || !fs::is_directory(directory)) {
            std::cerr << "Directory does not exist or is not a directory: " << dir << std::endl;
            continue;
        }

        // Рекурсивный обход всех файлов в директории
        for (const auto& entry : fs::recursive_directory_iterator(directory)) {
            // Пропускаем исключенные директории
            

            // Если это файл и его размер больше минимального, проверяем его
            if (fs::is_regular_file(entry) && fs::file_size(entry) >= min_size) {
                const auto& file_path = entry.path();

                // Проверяем, соответствует ли файл маске
                if (file_path.filename().string().find(file_mask) != std::string::npos) {
                    std::string file_hash = calculate_md5(file_path, block_size);
                    if (!file_hash.empty()) {
                        file_hash_map[file_hash].insert(file_path);
                    }
                }
            }
        }
    }

    // Выводим найденные дубликаты
    for (const auto& [hash, files] : file_hash_map) {
        if (files.size() > 1) {
            for (const auto& file : files) {
                std::cout << file << std::endl;
            }
            std::cout << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    // Пример данных для командной строки
    std::vector<std::string> dirs = {"C:/path/to/dir1", "C:/path/to/dir2"};
}
    
