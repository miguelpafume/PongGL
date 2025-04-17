#pragma once

#include <glad/glad.h>
#include <iostream>
#include "definitions.hpp"

// Return file's content as a string
std::string getFileContent(const char* filename);

class Shader {
	public:
		GLuint ID;

		void createShader(const char* vertex_file, const char* fragment_file);

		void Activate();
		void Delete();
};