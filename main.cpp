#include<iostream>
#include<string>
#include<vector>
#include<set>
#include<stack>
#include<unordered_map>
#include<fstream>
#include<boost/program_options.hpp>
#include<boost/filesystem.hpp>
#include<boost/regex.hpp>
#include<boost/crc.hpp>
#include<boost/uuid/detail/md5.hpp>
#include<boost/algorithm/hex.hpp>
namespace fs = boost::filesystem;
namespace po = boost::program_options;

void parse_arguments(int argc, char* argv[], std::vector<std::string>& dirs,
                    std::vector<std::string>&exclude_dirs,
                    int& scan_level, size_t& min_size,
                    std::string& file_mask, size_t& block_size,
                    std::string& hash_algorithm){
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("dirs,d", po::value<std::vector<std::string>>(&dirs)->multitoken(), "directories to scan")
        ("exclude,e", po::value<std::vector<std::string>>(&exclude_dirs)->multitoken(), "exclude directories")
        ("level,l", po::value<int>(&scan_level)->default_value(1), "scan level (0 for no recursion)")
        ("min-size,m", po::value<size_t>(&min_size)->default_value(1), "minimum file size")
        ("mask,M", po::value<std::string>(&file_mask)->default_value(".*"), "file mask")
        ("block-size,b", po::value<size_t>(&block_size)->default_value(4096), "block size for reading files")
        ("hash,h", po::value<std::string>(&hash_algorithm)->default_value("md5"), "hash algorithm (crc32/md5)"); //ключ-значение-описание
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    
    if(vm.count("help")){
        std::cout << desc <<std::endl;
        exit(0);
    }

}

void scan_directory(const std::vector<std::string>& dirs,
                    const std::set<fs::path>& exclude_paths,
                    int scan_level,
                    size_t min_size,
                    const boost::regex& file_mask,
                    size_t block_size, const std::string& algorithm,
                    std::unordered_map<std::string, std::vector<fs::path>>& hash_to_files);
std::string calculate_file_hash(const fs::path& file_path, size_t block_size, const std::string& algorithm);
std::string calculate_md5(const fs::path& file_path, size_t block_size);
std::string calculate_crc32(const fs::path& file_path, size_t block_size);
void print_duplicates(const std::unordered_map<std::string, std::vector<fs::path>>& hash_to_files);
int main(int argc, char* argv[]){
try {
        // Параметры командной строки
        std::vector<std::string> dirs;
        std::vector<std::string> exclude_dirs;
        int scan_level = 0;
        size_t min_size = 1;
        std::string file_mask = ".*", algorithm;
        size_t block_size = 4096;

        // Парсинг командной строки
        po::options_description desc("Options");
        desc.add_options()
            ("help,h", "Show help message")
            ("dirs,d", po::value<std::vector<std::string>>(&dirs)->multitoken(), "Directories to scan")
            ("exclude,e", po::value<std::vector<std::string>>(&exclude_dirs)->multitoken(), "Directories to exclude")
            ("level,l", po::value<int>(&scan_level)->default_value(0), "Scan level (0 = no recursion)")
            ("min-size,m", po::value<size_t>(&min_size)->default_value(1), "Minimum file size in bytes")
            ("file-mask,f", po::value<std::string>(&file_mask)->default_value(".*"), "File name mask (regex)")
            ("block-size,b", po::value<size_t>(&block_size)->default_value(4096), "Block size for reading files")
            ("algorithm,a", po::value<std::string>(&algorithm)->default_value("crc32"), "Hash algorithm (crc32 or md5)");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help") || dirs.empty()) {
            std::cout << desc << '\n';
            return 0;
        }
        boost::regex mask_regex(file_mask, boost::regex::icase);
        std::set<fs::path> exclude_paths(exclude_dirs.begin(), exclude_dirs.end());
        std::unordered_map<std::string, std::vector<fs::path>> hash_to_files;

        // Сканирование директории
        scan_directory(dirs, exclude_paths, scan_level, min_size, mask_regex, block_size, algorithm, hash_to_files);

        // Вывод результатов
        print_duplicates(hash_to_files);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }    

    return 0;
}
void scan_directory(const std::vector<std::string>& dirs,
                    const std::set<fs::path>& exclude_paths,
                    int scan_level,
                    size_t min_size,
                    const boost::regex& file_mask,
                    size_t block_size, const std::string& algorithm,
                    std::unordered_map<std::string, std::vector<fs::path>>& hash_to_files) {
    for (const auto& dir : dirs) {
        fs::path directory(dir);
        if (fs::exists(directory) && fs::is_directory(directory)) {
            std::stack<std::pair<fs::recursive_directory_iterator, int>> stack;
            stack.push({fs::recursive_directory_iterator(directory), 0});

            while (!stack.empty()) {
                auto [it, level] = stack.top();
                stack.pop();

                while (it != fs::recursive_directory_iterator()) {
                    const fs::path& current_path = it->path();

                    if (fs::is_directory(current_path)) {
                        if (exclude_paths.count(current_path)) {
                            ++it;
                            continue;
                        }

                        if (scan_level != 0 && level + 1 > scan_level) {
                            ++it;
                            continue;
                        }

                        stack.push({++it, level});
                        break;
                    }

                    if (fs::is_regular_file(current_path) &&
                        fs::file_size(current_path) >= min_size &&
                        boost::regex_match(current_path.filename().string(), file_mask)) {
                        try {
                            std::string hash = calculate_file_hash(current_path, block_size, algorithm);
                            hash_to_files[hash].push_back(current_path);
                        } catch (const std::exception& e) {
                            std::cerr << "Error processing file " << current_path << ": " << e.what() << '\n';
                        }
                    }

                    ++it;
                }
            }
        } else {
            std::cerr << "Directory does not exist or is not a directory: " << dir << '\n';
        }
    }
}


std::string calculate_file_hash(const fs::path& file_path, size_t block_size, const std::string& algorithm) {
    if (algorithm == "crc32") {
        return calculate_crc32(file_path, block_size);
    } else if (algorithm == "md5") {
        return calculate_md5(file_path, block_size);
    } else {
        throw std::invalid_argument("Unsupported hash algorithm: " + algorithm);
    }
}
std::string calculate_md5(const fs::path& file_path, size_t block_size)
{
    std::ifstream file(file_path.string(), std::ios::binary); // fs::path -> string
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + file_path.string());
    }

    boost::uuids::detail::md5 hash;
    boost::uuids::detail::md5::digest_type digest;
    std::vector<char> buffer(block_size);

    while (file.read(buffer.data(), block_size)) {
        hash.process_bytes(buffer.data(), file.gcount());
    }
    // Обрабатываем последний блок
    std::streamsize last_bytes_read = file.gcount(); // Сколько байт прочитано в последнем блоке
    if (last_bytes_read > 0) {
        // Заполняем остаток блока нулями
        std::fill(buffer.begin() + last_bytes_read, buffer.end(), 0);
        hash.process_bytes(buffer.data(), block_size); // Передаем весь блок с нулями
    }

    hash.get_digest(digest);

    const auto* digest_bytes = reinterpret_cast<const char*>(&digest);
    std::string md5_result;
    boost::algorithm::hex(digest_bytes, digest_bytes + sizeof(digest), std::back_inserter(md5_result));

    return md5_result;
}
std::string calculate_crc32(const fs::path& file_path, size_t block_size)
{
    std::ifstream file(file_path.string(), std::ios::binary); // fs::path -> string
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + file_path.string());
    }

    boost::crc_32_type crc;
    std::vector<char> buffer(block_size);

    //читаем блоками
    while (file.read(buffer.data(), block_size)) {
        crc.process_bytes(buffer.data(), file.gcount());
    }
     // Обрабатываем последний блок
    std::streamsize last_bytes_read = file.gcount(); // Сколько байт прочитано в последнем блоке
    if (last_bytes_read > 0) {
        // Заполняем остаток блока нулями
        std::fill(buffer.begin() + last_bytes_read, buffer.end(), 0);
        crc.process_bytes(buffer.data(), block_size); // Передаем весь блок с нулями
    }

    return std::to_string(crc.checksum());
}
void print_duplicates(const std::unordered_map<std::string, std::vector<fs::path>> &hash_to_files)
{
    for (const auto& [hash, paths] : hash_to_files) {
        if (paths.size() > 1) {
            for (const auto& path : paths) {
                std::cout << path.string() << '\n';
            }
            std::cout << '\n'; // Разделяем группы пустой строкой
        }
    }
}
