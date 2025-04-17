#pragma once

#include <glad/glad.h>
#include "definitions.hpp"

class VBO {
	public:
		GLuint ID;

		VBO(vec2* verticies, GLsizeiptr size, GLenum draw);
		VBO(GLfloat* verticies, GLsizeiptr size, GLenum draw);

		void Bind();
		void Unbind();
		void Delete();
};