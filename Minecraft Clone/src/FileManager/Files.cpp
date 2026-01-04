#include "FileManager/Files.h"

#include <stdio.h>

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <string>
#include <system_error>
#include <vector>

using std::string;
using std::vector;

vector<string> Tokenize(const string& str, char divider) {
  vector<string> tokens = {};
  size_t l = 0, r = 0;
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

File::File(const string& dir) { Open(dir); }

void File::Open(const string& dir) {
  dir_ = dir;
  file_.open(dir_);
}

void File::Close() { file_.close(); }

vector<string> File::GetToken(char divider) {
  std::ifstream f(dir_);

  vector<string> tokens = {};

  string line;

  while (std::getline(f, line)) {
    vector<string> lineTokens = Tokenize(line, divider);
    tokens.insert(tokens.end(), lineTokens.begin(), lineTokens.end());
  }

  return tokens;
}

bool FileManager::CheckFolder(const string& dir) {
  std::filesystem::path p = dir;

  std::error_code ec;  // To capture potential errors (optional)
  bool result = std::filesystem::exists(p, ec);

  return result && !ec;
}

bool FileManager::CreateFolder(const string& name) {
  if (CheckFolder(name)) {
    return false;  // Already exist
  }

  return (std::filesystem::create_directory(name) == 0);
}

// Return true of file exist and false if it doesn't
bool FileManager::CheckFile(const string& dir) {
  return std::filesystem::exists(dir);
}

void FileManager::CreateFile(const string& name, const string& dir) {
  string d = "";

  if (dir == "N/A")
    d = name;
  else
    d = (dir + "//" + name);
  std::ofstream file(d.c_str());
  file.close();
}

File FileManager::GetFile(const string& name, const string& dir) {
  string d = "";

  if (dir == "N/A")
    d = name;
  else
    d = (dir + "//" + name);

  return File(d);
}

void FileManager::DeleteFile(const string& name, const string& dir) {
  string d = "";

  if (dir == "N/A")
    d = name;
  else
    d = (dir + "//" + name);

  remove(d.c_str());
}