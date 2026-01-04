#pragma once
#include <fstream>
#include <string>
#include <vector>

std::vector<std::string> Tokenize(const std::string& str, char divider);

class File {
 public:
  std::ifstream file_;
  std::string dir_ = "";

  File();
  File(const std::string& dir);

  void Open(const std::string& dir);

  void Close();

  std::vector<std::string> GetToken(char divider);
};

class FileManager {
 public:
  static bool CheckFolder(const std::string& dir);

  static bool CreateFolder(const std::string& name);

  // Return true of file exist and false if it doesn't
  static bool CheckFile(const std::string& dir);

  static void CreateFile(const std::string& name,
                         const std::string& dir = "N/A");

  static File GetFile(const std::string& name, const std::string& dir = "N/A");

  static void DeleteFile(const std::string& name,
                         const std::string& dir = "N/A");
};