#pragma once
#include <fstream>
#include <string>
#include <vector>

std::vector<std::string> Tokenize(std::string str, char divider);

class File {
 public:
  std::ifstream file_;
  std::string dir_ = "";

  File();
  File(std::string dir);

  void Open(std::string dir);

  void Close();

  std::vector<std::string> GetToken(char divider);
};

class FileManager {
 public:
  static bool CheckFolder(std::string dir);

  static bool CreateFolder(std::string name);

  // Return true of file exist and false if it doesn't
  static bool CheckFile(std::string dir);

  static void CreateFile(std::string name, std::string dir = "N/A");

  static File GetFile(std::string name, std::string dir = "N/A");

  static void DeleteFile(std::string name, std::string dir = "N/A");
};