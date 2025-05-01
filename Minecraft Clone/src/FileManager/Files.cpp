#include "FileManager/Files.h"

#include <filesystem>
#include <sstream>

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

File::File(std::string dir) { Open(dir); }

void File::Open(std::string dir) {
  dir_ = dir;
  file_.open(dir_);
}

void File::Close() { file_.close(); }

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

bool FileManager::CheckFolder(std::string dir) {
  std::filesystem::path p = dir;

  std::error_code ec;  // To capture potential errors (optional)
  bool result = std::filesystem::exists(p, ec);

  return result && !ec;
}

bool FileManager::CreateFolder(std::string name) {
  if (CheckFolder(name)) {
    return false;  // Already exist
  }

  return (std::filesystem::create_directory(name) == 0);
}

// Return true of file exist and false if it doesn't
bool FileManager::CheckFile(std::string dir) {
  return std::filesystem::exists(dir);
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