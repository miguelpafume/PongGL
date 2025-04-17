#include "definitions.hpp"

std::string getFileContent(const char* filename) {
	std::ifstream file(filename, std::ios::binary);

	if (file) {
		std::string contents;

		file.seekg(0, std::ios::end);
		contents.resize(file.tellg());

		file.seekg(0, std::ios::beg);
		file.read(&contents[0], contents.size());

		file.close();
		return contents;
	}

	// If couldn't open file
	throw(errno);
};