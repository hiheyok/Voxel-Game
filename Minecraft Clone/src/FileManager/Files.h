#pragma once

#include <sys/stat.h>
#include <direct.h>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

std::vector<std::string> Tokenize(std::string str, char divider);

class File{
public:

	std::ifstream file_;
	std::string dir_ = "";

	File() {

	}

	File(std::string dir) {
		Open(dir);
	}

	void Open(std::string dir);

	void Close();

	std::vector<std::string> GetToken(char divider);
};

class FileManager {
public:
	static bool CheckFolder(const char* dir);

	static bool CreateFolder(const char* name);

	//Return true of file exist and false if it doesn't
	static bool CheckFile(const char* dir);

	static bool CheckFile(std::string dir);

	static void CreateFile(std::string name, std::string dir = "N/A");

	static File GetFile(std::string name, std::string dir = "N/A");

	static void DeleteFile(std::string name, std::string dir = "N/A");
};