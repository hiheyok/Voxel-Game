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

	std::ifstream file;
	std::string dir = "";

	File() {

	}

	File(std::string dir_) {
		open(dir_);
	}

	void open(std::string dir_);

	void close();

	std::vector<std::string> GetToken(char Divider);
};

class FileManager {
public:
	static bool CheckFolder(const char* dir);

	static bool CreateFolder(const char* name);

	//Return true of file exist and false if it doesn't
	static bool CheckFile(const char* dir);

	static void CreateFile(std::string name, std::string dir = "N/A");

	static File GetFile(std::string name, std::string dir = "N/A");

	static void DeleteFile(std::string name, std::string dir = "N/A");
};