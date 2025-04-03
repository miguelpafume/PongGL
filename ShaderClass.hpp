#pragma once

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno>

std::string getFileContent(const char* filename);

class Shader {
	public:
		GLuint ID;
		Shader(const char* vertex_file, const char* fragment_file);

		void Activate();
		void Delete();
};