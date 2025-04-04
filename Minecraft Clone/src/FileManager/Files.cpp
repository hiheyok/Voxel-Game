#include <filesystem>
#include <sstream>
#include "FileManager/Files.h"

std::vector<std::string> Tokenize(std::string str, char divider) {
    std::vector<std::string> tokens = {};
    int l = 0, r = 0;
    while (r < str.size()) {
        // Shift right ptr to the first non-divider character

        while (r < str.size() && str[r] != divider) r++;
        tokens.push_back(str.substr(l, r));
        l = r + 1;
        r++;
    }

    return tokens;
}

File::File() = default;

File::File(std::string dir) {
    Open(dir);
}

void File::Open(std::string dir) {
    dir_ = dir;
    file_.open(dir_);
}

void File::Close() {
    file_.close();
}

std::vector<std::string> File::GetToken(char divider) {

    std::ifstream f(dir_);

    std::vector<std::string> tokens = {};

    std::string line;

    while (std::getline(f, line)) {

        std::vector<std::string> lineTokens = Tokenize(line, divider);
        tokens.insert(tokens.end(), lineTokens.begin(), lineTokens.end());
    }

    return tokens;
}

bool FileManager::CheckFolder(const char* dir) {
    struct stat buf;
    return stat(dir, &buf) == 0;
}

bool FileManager::CreateFolder(const char* name) {
    if (CheckFolder(name)) {
        return false; //Already exist
    }

    return (mkdir(name) == 0);
}

//Return true of file exist and false if it doesn't
bool FileManager::CheckFile(const char* dir) {
    return std::filesystem::exists(dir);

}

bool FileManager::CheckFile(std::string dir) {
    return CheckFile(dir.c_str());
}

void FileManager::CreateFile(std::string name, std::string dir) {
    std::string d = "";

    if (dir == "N/A")
        d = name;
    else
        d = (dir + "//" + name);
    std::ofstream file(d.c_str());
    file.close();
}

File FileManager::GetFile(std::string name, std::string dir) {
    std::string d = "";

    if (dir == "N/A")
        d = name;
    else
        d = (dir + "//" + name);


    return File(d);
}

void FileManager::DeleteFile(std::string name, std::string dir) {
    std::string d = "";

    if (dir == "N/A")
        d = name;
    else
        d = (dir + "//" + name);

    remove(d.c_str());
}