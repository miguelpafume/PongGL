#pragma once

#include <glad/glad.h>
#include "VBO.hpp"

class VAO {
	public: 
		GLuint ID;
		VAO();

		// Link passed VBO to the current VAO object
		void linkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset, GLuint divisor);

		void Bind();
		void Unbind();
		void Delete();
};