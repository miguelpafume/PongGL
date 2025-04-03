#include "Texture.hpp"

Texture::Texture(const char* image, GLenum texture_type, GLenum slot, GLenum format, GLenum pixel_type) {
	type = texture_type;

	int width_img, height_img, col_channels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* bytes = stbi_load(image, &width_img, &height_img, &col_channels, 0);

	glGenTextures(1, &ID);
	glActiveTexture(slot);
	glBindTexture(texture_type, ID);

	glTexParameteri(texture_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(texture_type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(texture_type, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(texture_type, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(texture_type, 0, GL_RGB, width_img, height_img, 0, format, pixel_type, bytes);
	glGenerateMipmap(texture_type);

	stbi_image_free(bytes);
	glBindTexture(texture_type, 0);
}

void Texture::textureUnit(Shader& shader, const char* uniform, GLuint unit) {
	GLuint textureUni = glGetUniformLocation(shader.ID, uniform);
	shader.Activate();
	glUniform1i(textureUni, unit);
}

void Texture::Bind() {
	glBindTexture(type, ID);
}

void Texture::Unbind() {
	glBindTexture(type, 0);
}

void Texture::Delete() {
	glDeleteTextures(1, &ID);
}