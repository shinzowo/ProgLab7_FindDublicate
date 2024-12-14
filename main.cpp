#include<iostream>

int main(){
    std::cout << "Options:\n"
          << "  -h [ --help ]                   Show help message\n"
          << "  -d [ --dirs ] arg               Directories to scan\n"
          << "  -e [ --exclude ] arg            Directories to exclude\n"
          << "  -l [ --level ] arg (=0)         Scan level (0 = no recursion)\n"
          << "  -m [ --min-size ] arg (=1)      Minimum file size in bytes\n"
          << "  -f [ --file-mask ] arg (=.*)    File name mask (regex)\n"
          << "  -b [ --block-size ] arg (=4096) Block size for reading files\n"
          << "  -a [ --algorithm ] arg (=crc32) Hash algorithm (crc32 or md5)\n";

}