#pragma once

#include <glad/glad.h>
#include "VBO.hpp"

class VAO {
	public: 
		GLuint ID;
		VAO();

		void LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset, GLuint divisor);

		void Bind();
		void Unbind();
		void Delete();
};