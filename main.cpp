#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>	

#include "ShaderClass.hpp"
#include "VBO.hpp"
#include "VAO.hpp"
#include "EBO.hpp"

GLuint SCREEN_WIDTH = 800;
GLuint SCREEN_HEIGHT = 600;
Shader SHADER;

//Sets triangles points on screen
GLfloat vertices[] = {
	0.5f,  0.5f,
	-0.5f,  0.5f,
	-0.5f, -0.5f,
	0.5f, -0.5f
};

GLuint indices[] = {
	0, 1, 2,
	2, 3, 0
};

GLfloat offsets[] = {
	200.0f, 200.0f
};

GLfloat sizes[]{
	100.0f, 100.0f
};

void setOrthographicProjection(Shader shader_program,
	float left, float right,
	float bottom, float top,
	float near, float far) {

	float mat[4][4] = {
	{ 2.0f / (right - left), 0.0f, 0.0f, 0.0f },
	{ 0.0f, 2.0f / (top - bottom), 0.0f, 0.0f },
	{ 0.0f, 0.0f, -2.0f / (far - near), 0.0f },
	{ -(right + left) / (right - left), -(top + bottom) / (top - bottom), -(far + near) / (far - near), 1.0f }
	};

	shader_program.Activate();
	glUniformMatrix4fv(glGetUniformLocation(shader_program.ID, "projection"), 1, GL_FALSE, &mat[0][0]);
};

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;

	setOrthographicProjection(SHADER, 0, width, 0, height, 0.0f, 1.0f);
};

void processInput(GLFWwindow* window);

int main() {
	//INITIALIZE OPENGL VERSION 4.6 
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Set windows to a 800x800 square
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "PongGL", NULL, NULL);

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	//Checks if window was created
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Loads and checks for proper loading of GLAD
	gladLoadGL();
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//Generates the shader object
	SHADER.createShader("default.vert", "default.frag");
	setOrthographicProjection(SHADER, 0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, 0.0f, 1.0f);

	//Generates Vertex Array Object and binds it
	VAO vao;
	vao.Bind();

	//Generates Vertex Buffer Object and binds it
	VBO position_vbo(vertices, sizeof(vertices), GL_STATIC_DRAW);
	VBO offset_vbo(offsets, sizeof(offsets), GL_DYNAMIC_DRAW);
	VBO size_vbo(sizes, sizeof(sizes), GL_DYNAMIC_DRAW);

	//Generates Element Buffer Object and binds it
	EBO ebo(indices, sizeof(indices));

	vao.LinkAttrib(position_vbo, 0, 2, GL_FLOAT, 2 * sizeof(GLfloat), (void*)0, 0);
	vao.LinkAttrib(offset_vbo, 1, 2, GL_FLOAT, 2 * sizeof(GLfloat), (void*)0, 1);
	vao.LinkAttrib(size_vbo, 2, 2, GL_FLOAT, 2 * sizeof(GLfloat), (void*)0, 2);

	//Unbind all previous objects
	vao.Unbind();
	ebo.Unbind();

	//Main program loop
	while (!glfwWindowShouldClose(window)) {
		//INPUT
		processInput(window);

		//RENDERING
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//Sets the shader program to the previous one made
		SHADER.Activate();

		vao.Bind();

		//Draws the triangle
		glBindVertexArray(vao.ID);
		glDrawElementsInstanced(GL_TRIANGLES, sizeof(indices)/sizeof(int), GL_UNSIGNED_INT, 0, 1);

		//Check and call events & swap buffers
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	//Properly deletes stuff in memory and exits program
	vao.Delete();
	ebo.Delete();

	position_vbo.Delete();
	offset_vbo.Delete();
	size_vbo.Delete();

	SHADER.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
}