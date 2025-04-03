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

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
//void setOrthographicProjection(	Shader shaderProgram,
//								float left, float right,
//								float bottom, float top,
//								float near, float far);

GLint SCREEN_WIDTH = 800;
GLint SCREEN_HEIGHT = 800;

//Sets triangles points on screen
GLfloat vertices[] = {
   -0.5f,   0.5f, //UP LEFT
	0.5f,   0.5f, //UP RIGHT
   -0.5f,  -0.5f, //DOWN LEFT
    0.5f,  -0.5f  //DOWN RIGHT
};

GLuint indices[] = {
	0, 1, 2,
	1, 3, 2
};

GLfloat offsets[] {
	200.0f, 200.0f
};

GLfloat sizes[]{
	50.0f, 50.0f
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

int main() {
	//INITIALIZE OPENGL VERSION 4.6 
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Set windows to a 800x800 square
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pong", NULL, NULL);

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

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
	Shader ShaderProgram("default.vert", "default.frag");

	//Generates Vertex Array Object and binds it
	VAO VAO_1;
	VAO_1.Bind();

	//Generates Vertex Buffer Object and binds it
	VBO position_vbo(vertices, sizeof(vertices), GL_STATIC_DRAW);
	VBO offset_vbo(offsets, sizeof(offsets), GL_DYNAMIC_DRAW);
	VBO size_vbo(sizes, sizeof(sizes), GL_DYNAMIC_DRAW);

	//Generates Element Buffer Object and binds it
	EBO EBO_1(indices, sizeof(indices));

	VAO_1.LinkAttrib(position_vbo, 0, 2, GL_FLOAT, 2 * sizeof(GLfloat), (void*)0);
	VAO_1.LinkAttrib(offset_vbo, 1, 2, GL_FLOAT, 2 * sizeof(GLfloat), (void*)0);
	VAO_1.LinkAttrib(size_vbo, 2, 2, GL_FLOAT, 2 * sizeof(GLfloat), (void*)0);

	//Unbind all previous objects
	VAO_1.Unbind();
	EBO_1.Unbind();

	//Main program loop
	while (!glfwWindowShouldClose(window)) {
		//INPUT

		processInput(window);

		//RENDERING
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//Sets the shader program to the previous one made
		ShaderProgram.Activate();
		setOrthographicProjection();

		VAO_1.Bind();

		//Draws the triangle
		glBindVertexArray(VAO_1.ID);
		glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(int), GL_UNSIGNED_INT, 0);

		//Check and call events & swap buffers
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	//Properly deletes stuff in memory and exits program
	VAO_1.Delete();
	position_vbo.Delete();
	EBO_1.Delete();

	offset_vbo.Delete();

	ShaderProgram.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
}