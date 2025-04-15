#include "VAO.hpp"

VAO::VAO() {
	glGenVertexArrays(1, &ID);
}

// Link passed VBO to the current VAO object
void VAO::LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset, GLuint divisor) {
	VBO.Bind();
	glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
	glEnableVertexAttribArray(layout);

	if (divisor > 0) {
		glVertexAttribDivisor(layout, divisor);
	}

	VBO.Unbind();
}

void VAO::Bind() {
	glBindVertexArray(ID);
}

void VAO::Unbind() {
	glBindVertexArray(0);
}

void VAO::Delete() {
	glDeleteVertexArrays(1, &ID);
}