#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <cerrno>
#include <glad/glad.h>

struct vec2 {
	GLfloat x;
	GLfloat y;
};

// Return file's content as a string
std::string getFileContent(const char* filename);