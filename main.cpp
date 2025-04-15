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

float PI = 4 * atanf(1.0f);

// Creates a circle using a 2D array for indices and precision (num_triangles AKA slices)
void gen2DCircleArray(float*& vertices, unsigned int*& indices, unsigned int num_triangles, float radius = 1.0f) {
	// Empty array for triangles points
	vertices = new GLfloat[(num_triangles + 1) * 2];

	// Center point
	vertices[0] = 0.0f;
	vertices[1] = 0.0f;

	// Empty indice to make the triangles
	indices = new GLuint[num_triangles * 3];

	// Angle of every triangle for the circle
	float theta = 0.0f;

	// Assign values on the arrays
	for (unsigned int i = 0; i < num_triangles; i++) {
		vertices[(i + 1) * 2] = radius * cosf(theta);
		vertices[(i + 1) * 2 + 1] = radius * sinf(theta);

		indices[i * 3 + 0] = 0;
		indices[i * 3 + 1] = i + 1;
		indices[i * 3 + 2] = i + 2;

		theta += 2 * PI / num_triangles;
	}

	indices[(num_triangles - 1) * 3 + 2] = 1;
}

void setOrthographicProjection(Shader shader_program,
	float left, float right,
	float bottom, float top,
	float near, float far) {

	// Orthographic matrix
	float mat[4][4] = {
	{ 2.0f / (right - left), 0.0f, 0.0f, 0.0f },
	{ 0.0f, 2.0f / (top - bottom), 0.0f, 0.0f },
	{ 0.0f, 0.0f, -2.0f / (far - near), 0.0f },
	{ -(right + left) / (right - left), -(top + bottom) / (top - bottom), -(far + near) / (far - near), 1.0f }
	};

	shader_program.Activate();

	// Set the orthographic matrix to be used by the shader projection
	glUniformMatrix4fv(glGetUniformLocation(shader_program.ID, "projection"), 1, GL_FALSE, &mat[0][0]);
};

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	// Updates windows width and height
	glViewport(0, 0, width, height);
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;

	// Set projection based on current windows size
	setOrthographicProjection(SHADER, 0, width, 0, height, 0.0f, 1.0f);
};

void processInput(GLFWwindow* window, GLfloat *offset, float delta_time) {
	float move_speed = 50.0f; // ### RENAME TO PADDLE SPEED (MAYBE BALL SPEED TOO)

	// Closes the windows when escape is pressed
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

	// ### TEMP
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) { 
		offset[1] += move_speed * delta_time;
	};
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		offset[1] -= move_speed * delta_time;
	};
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		offset[0] += move_speed * delta_time;
	};
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		offset[0] -= move_speed * delta_time;
	};
};

int main() {
	// Initialize OpenGL version 4.6 
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create window instance and makes it a 800x600 pixel res
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "PongGL", NULL, NULL);

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	// Checks if window was properly created
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

	// Generates the shader object using vertex and fragment shader files
	SHADER.createShader("default.vert", "default.frag");
	setOrthographicProjection(SHADER, 0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, 0.0f, 1.0f);

	// Ball variables
	GLfloat* ball_vertices;
	GLuint* ball_indices;
	unsigned int num_triangles = 15; // Precision

	// Assign values for the ball's arrays
	gen2DCircleArray(ball_vertices, ball_indices, num_triangles, 0.5f);

	// More ball arrays
	GLfloat ball_offsets[] = {
		SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f
	};

	GLfloat ball_sizes[]{
		10.0f, 10.0f
	};

	// Generates ball Vertex Array Object and binds it
	VAO ball_VAO;
	ball_VAO.Bind();

	// Generates ball Vertex Buffer Objects and binds them
	VBO ball_position_vbo(ball_vertices, 2 * (num_triangles + 1) * sizeof(GLfloat), GL_STATIC_DRAW);
	VBO ball_offset_vbo(ball_offsets, sizeof(ball_offsets), GL_DYNAMIC_DRAW);
	VBO ball_size_vbo(ball_sizes, sizeof(ball_sizes), GL_DYNAMIC_DRAW);

	// Generates Element Buffer Object and binds it
	EBO ebo(ball_indices, 3 * num_triangles * sizeof(GLfloat));
	ebo.Bind();

	// Links all of ball's VBOs to it's VAO
	ball_VAO.LinkAttrib(ball_position_vbo, 0, 2, GL_FLOAT, 2 * sizeof(GLfloat), (void*)0, 0);
	ball_VAO.LinkAttrib(ball_offset_vbo, 1, 2, GL_FLOAT, 2 * sizeof(GLfloat), (void*)0, 1);
	ball_VAO.LinkAttrib(ball_size_vbo, 2, 2, GL_FLOAT, 2 * sizeof(GLfloat), (void*)0, 2);

	// Unbind all previous objects
	ball_VAO.Unbind();
	ball_position_vbo.Unbind();
	ball_offset_vbo.Unbind();
	ball_size_vbo.Unbind();
	ebo.Unbind();

	// Time elapse, used to stabilyze movement across different framerates
	float delta_time = 0.0f, last_frame = 0.0f;

	// Main program loop
	while (!glfwWindowShouldClose(window)) {
		// Time elapsed since last frame
		float current_frame = glfwGetTime();
		delta_time = current_frame - last_frame;
		last_frame = current_frame;

		processInput(window, ball_offsets, delta_time);

		// Clear screen and set background to black
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Activates render/shader object
		SHADER.Activate();

		// Draw the ball on screen
		ball_offset_vbo.Bind();
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(ball_offsets), ball_offsets);

		// Updates ball
		ball_VAO.Bind();
		glDrawElementsInstanced(GL_TRIANGLES, 3 * num_triangles, GL_UNSIGNED_INT, 0, 1);

		// Swap frames
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Clears up everything
	ball_VAO.Delete();
	ebo.Delete();

	ball_position_vbo.Delete();
	ball_offset_vbo.Delete();
	ball_size_vbo.Delete();

	SHADER.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}