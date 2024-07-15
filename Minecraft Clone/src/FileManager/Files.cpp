#include "Files.h"

std::vector<std::string> Tokenize(std::string str, char divider) {
	std::vector<std::string> Tokens = {};

	std::stringstream check(str);

	std::string intermediate;

	while (std::getline(check, intermediate, divider)) {
		Tokens.push_back(intermediate);
	}

	return Tokens;
}

void File::open(std::string dir_) {
	dir = dir_;
	file.open(dir_);
}

void File::close() {
	file.close();
}

std::vector<std::string> File::GetToken(char Divider) {

	std::ifstream f(dir);

	std::vector<std::string> Tokens = {};

	std::string line;

	while (std::getline(f, line)) {

		std::vector<std::string> lineTokens = Tokenize(line, Divider);
		Tokens.insert(Tokens.end(), lineTokens.begin(), lineTokens.end());
	}

	return Tokens;
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
	std::ifstream file;
	file.open(dir);

	if (file)
		return true;
	else
		return false;

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