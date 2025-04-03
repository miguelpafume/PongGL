#include "ShaderClass.hpp"

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

	throw(errno);
};


Shader::Shader(const char* vertex_file, const char* fragment_file) {
	std::string vertex_code = getFileContent(vertex_file);
	std::string fragment_code = getFileContent(fragment_file);

	const char* vertex_source = vertex_code.c_str();
	const char* fragment_source = fragment_code.c_str();

	//Vertex shader compiling
	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vertex_shader, 1, &vertex_source, NULL);
	glCompileShader(vertex_shader);

	//Checks for vertex shader compilation
	int  success;
	char infoLog[512];

	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//Fragament shader compiling
	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(fragment_shader, 1, &fragment_source, NULL);
	glCompileShader(fragment_shader);

	//Checks for fragment shader compiling
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//Shader Program
	ID = glCreateProgram();

	//Attaches previous shader to shader program
	glAttachShader(ID, vertex_shader);
	glAttachShader(ID, fragment_shader);
	glLinkProgram(ID);

	//Checks for shader program
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	//Delete vertex and fragment shader from memory
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
}

void Shader::Activate() {
	glUseProgram(ID);
}

void Shader::Delete() {
	glDeleteProgram(ID);
}