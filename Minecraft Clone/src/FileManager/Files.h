#pragma once
#include <sys/stat.h>
#include <direct.h>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

namespace FileMgr {
	//struct stat ab;
}

class File{
private:
	

	std::vector<std::string> Tokenize(std::string str, char divider) {
		std::vector<std::string> Tokens = {};

		std::stringstream check(str);

		std::string intermediate;

		while (std::getline(check, intermediate, divider)) {
			Tokens.push_back(intermediate);
		}

		return Tokens;
	}

public:

	std::ifstream file;
	std::string dir = "";

	File() {

	}

	File(std::string dir_) {
		
		open(dir_);
	}


	void open(std::string dir_) {
		dir = dir_;
		file.open(dir_);
	}

	void close() {
		file.close();
	}

	std::string getNextLine() {

	}

	std::vector<std::string> GetToken(char Divider) {

		std::ifstream f(dir);

		std::vector<std::string> Tokens = {};

		std::string line;

		while (std::getline(f, line)) {
			
			std::vector<std::string> lineTokens = Tokenize(line, Divider);
			Tokens.insert(Tokens.end(), lineTokens.begin(), lineTokens.end());
		}

		return Tokens;

	}


};

class FileManager {
public:
	static bool CheckFolder(const char* dir) {
		struct stat buf;
		return stat(dir, &buf) == 0;
	}

	static bool CreateFolder(const char* name) {
		if (CheckFolder(name)) {
			return false; //Already exist
		}

		return (mkdir(name) == 0);
	}

	//Return true of file exist and false if it doesn't
	static bool CheckFile(const char* dir) {
		std::ifstream file;
		file.open(dir);

		if (file)
			return true;
		else
			return false;
		
	}

	static bool CreateFile(std::string name, std::string dir = "N/A") {
		std::string d = "";

		if (dir == "N/A")
			d = name;
		else
			d = (dir + "//" + name);

		std::ofstream file(d.c_str());
		file.close();
	}

	static File GetFile(std::string name, std::string dir = "N/A") {
		std::string d = "";

		if (dir == "N/A")
			d = name;
		else
			d = (dir + "/" + name);


		return File(d);
	}
};