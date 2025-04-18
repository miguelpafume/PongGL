#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

class VBO {
	public:
		GLuint ID;

		VBO(glm::vec2* verticies, GLsizeiptr size, GLenum draw);
		VBO(GLfloat* verticies, GLsizeiptr size, GLenum draw);

		void Bind();
		void Unbind();
		void Delete();
};