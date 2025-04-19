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

const float PI = 4 * atanf(1.0f);

const float ball_diameter = 14.0f;
const float ball_radius = ball_diameter / 2.0f;

const float paddle_height = 80.0f;
const float paddle_width = 12.0f;
const float paddle_speed = 150.0f;
const float paddle_boundary = (paddle_height / 2.0f) + (ball_diameter / 2.0f);

glm::vec2 paddle_offsets[2];
glm::vec2 ball_offset;

const float ball_min_velocity = 20.0f;
const float ball_max_velocity = 300.0f;

GLfloat paddle_velocity[2];

// Random number generator
int randomNumber(int min, int max, bool negative = false) {
	if (!negative && (min < 0 || max < 0)) {
		throw std::invalid_argument("CAN'T GENERATE NEGATIVE NUMBER IF NEGATIVES ARE NOT ALLOWED!");
	}

	if (negative) { 
		return (rand() % 1) ? -(rand() % (max - min) + min) : (rand() % (max - min) + min);
	}
	return (rand() % (max - min) + min);
}

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
	int left, float right,
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

	// Update paddle position
	paddle_offsets[1].x = width - 35.0f;
};

void processInput(GLFWwindow* window) {
	// Closes the windows when escape is pressed
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

	paddle_velocity[0] = 0.0f;
	paddle_velocity[1] = 0.0f;

	// Left paddle
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		if (paddle_offsets[0].y < SCREEN_HEIGHT - paddle_boundary) {
			paddle_velocity[0] = paddle_speed;
		}
		else {
			paddle_offsets[0].y = SCREEN_HEIGHT - paddle_boundary;
		}
	};
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		if (paddle_offsets[0].y > paddle_boundary) {
			paddle_velocity[0] = -paddle_speed;
		}
		else {
			paddle_offsets[0].y = paddle_boundary;
		}
	};

	// Right paddle
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) { 
		if (paddle_offsets[1].y < SCREEN_HEIGHT - paddle_boundary) {
			paddle_velocity[1] = paddle_speed;
		}
		else {
			paddle_offsets[1].y = SCREEN_HEIGHT - paddle_boundary;
		}
	};

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		if (paddle_offsets[1].y > paddle_boundary) {
			paddle_velocity[1] = -paddle_speed;
		}
		else {
			paddle_offsets[1].y = paddle_boundary;
		}
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

	// ***************
	// **	PADDLE	**
	// ***************
	
	GLfloat paddle_vertices[] = {
		 0.5f,  0.5f,
		-0.5f,  0.5f,
		-0.5f, -0.5f,
		 0.5f, -0.5f
	};

	GLuint paddle_indices[] = {
		0, 1, 2,
		2, 3 ,0
	};

	paddle_offsets[0] = { 35.0f, SCREEN_HEIGHT / 2.0f };
	paddle_offsets[1] = { SCREEN_WIDTH - 35.0f, SCREEN_HEIGHT / 2.0f };

	glm::vec2 paddle_sizes = { paddle_width, paddle_height };

	VAO paddle_vao;
	paddle_vao.Bind();

	VBO paddle_position_vbo(paddle_vertices, sizeof(paddle_vertices), GL_STATIC_DRAW);
	VBO paddle_offset_vbo(paddle_offsets, sizeof(paddle_offsets), GL_DYNAMIC_DRAW);
	VBO paddle_size_vbo(&paddle_sizes, sizeof(paddle_sizes), GL_STATIC_DRAW);

	EBO paddle_ebo(paddle_indices, sizeof(paddle_indices));
	paddle_ebo.Bind();

	paddle_vao.linkAttrib(paddle_position_vbo, 0, 2, GL_FLOAT, 2 * sizeof(GLfloat), (void*)0, 0);
	paddle_vao.linkAttrib(paddle_offset_vbo, 1, 2, GL_FLOAT, 2 * sizeof(GLfloat), (void*)0, 1);
	paddle_vao.linkAttrib(paddle_size_vbo, 2, 2, GL_FLOAT, 2 * sizeof(GLfloat), (void*)0, 2);

	paddle_vao.Unbind();
	paddle_position_vbo.Unbind();
	paddle_offset_vbo.Unbind();
	paddle_size_vbo.Unbind();
	paddle_ebo.Unbind();

	// ***************
	// **	BALL	**
	// ***************

	// Ball variables
	srand(time(0));
	glm::vec2 ball_velocity = { randomNumber(50, 150, true), randomNumber(0, 150, true) };
	
	GLfloat* ball_vertices;
	GLuint* ball_indices;
	unsigned int num_triangles = 15; // Precision

	// Assign values for the ball's info
	gen2DCircleArray(ball_vertices, ball_indices, num_triangles, 0.5f);

	// More ball informations
	ball_offset = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };

	glm::vec2 ball_size = { ball_diameter, ball_diameter };

	// Generates ball Vertex Array Object and binds it
	VAO ball_vao;
	ball_vao.Bind();

	// Generates ball Vertex Buffer Objects and binds them
	VBO ball_position_vbo(ball_vertices, 2 * (num_triangles + 1) * sizeof(GLfloat), GL_STATIC_DRAW);
	VBO ball_offset_vbo(&ball_offset, sizeof(ball_offset), GL_DYNAMIC_DRAW);
	VBO ball_size_vbo(&ball_size, sizeof(ball_size), GL_DYNAMIC_DRAW);

	// Generates Element Buffer Object and binds it
	EBO ball_ebo(ball_indices, 3 * num_triangles * sizeof(GLfloat));
	ball_ebo.Bind();

	// Links all of ball's VBOs to it's VAO
	ball_vao.linkAttrib(ball_position_vbo, 0, 2, GL_FLOAT, 2 * sizeof(GLfloat), (void*)0, 0);
	ball_vao.linkAttrib(ball_offset_vbo, 1, 2, GL_FLOAT, 2 * sizeof(GLfloat), (void*)0, 1);
	ball_vao.linkAttrib(ball_size_vbo, 2, 2, GL_FLOAT, 2 * sizeof(GLfloat), (void*)0, 2);

	// Unbind all previous objects
	ball_vao.Unbind();
	ball_position_vbo.Unbind();
	ball_offset_vbo.Unbind();
	ball_size_vbo.Unbind();
	ball_ebo.Unbind();

	// Time elapse, used to stabilyze movement across different framerates
	float dt = 0.0f, last_frame = 0.0f;

	// Collision frames
	const int collision_threshold = 3;
	int collision_cooldown = 0;

	// Which side scored, left (0) or right (1);
	bool winner = 0;

	// Main program loop
	while (!glfwWindowShouldClose(window)) {
		// Time elapsed since last frame
		float current_frame = (float)glfwGetTime();
		dt = current_frame - last_frame;
		last_frame = current_frame;

		bool reset = false;

		processInput(window);

		// *******************
		// **	COLLISIONS	**
		// *******************

		// Collision with top or bottom wall
		if (ball_offset.y - ball_radius <= 0 || ball_offset.y + ball_radius >= SCREEN_HEIGHT) {
			ball_velocity.y *= -1;

			float push = 0.1f * (ball_offset.y > SCREEN_HEIGHT / 2 ? -1 : 1);
			ball_offset.y += push;
		}

		// Collision with left wall 
		if (ball_offset.x - ball_radius <= 0) {
			winner = 0;
			reset = true;
		}

		// Collision with right wall 
		if (ball_offset.x + ball_radius >= SCREEN_WIDTH) {
			winner = 1;
			reset = true;
		}

		// Centers ball and reset velocity
		if (reset) {
			if (winner) {
				ball_velocity = { -randomNumber(50, 150), randomNumber(0, 150, true) };
			}
			else {
				ball_velocity = { randomNumber(50, 150), randomNumber(0, 150, true) };
			}

			ball_offset.x = SCREEN_WIDTH / 2.0f;
			ball_offset.y = SCREEN_HEIGHT / 2.0f;
		}

		// Paddle collision
		if (collision_cooldown > 0) {
			collision_cooldown--;
		}

		if (collision_cooldown == 0) {
			//Checks for left (0) and right (1) paddle
			for (int lr = 0; lr < 2; lr++) {
				// Calculate distance vector
				glm::vec2 distance = {
					std::abs(ball_offset.x - paddle_offsets[lr].x) - (paddle_width / 2 + ball_radius),
					std::abs(ball_offset.y - paddle_offsets[lr].y) - (paddle_height / 2 + ball_radius)
				};

				// If both distances are negative the ball has a collision
				if (distance.x < 0 && distance.y < 0) {
					// Determine which side was hit
					if (distance.x > distance.y) {
						// Horizontal collision (left/right of paddle)
						ball_velocity.x *= -1;

						// Push ball out to prevent sticking
						float push = (distance.x + 0.1f) * (ball_offset.x < paddle_offsets[lr].x ? -1 : 1);
						ball_offset.x += push;
					}
					else {
						// Vertical collision (top/bottom of paddle)
						ball_velocity.y *= -1;
					
						// Push ball out to prevent sticking
						float push = (distance.y + 0.1f) * (ball_offset.y < paddle_offsets[lr].y ? -1 : 1);
						ball_offset.y += push;
					}

					// Speed up ball
					ball_velocity.x *= 1.05f;
					ball_velocity.y += 0.5f * paddle_velocity[lr];

					// Checks for ball minimum and maximum velocities
					if (std::abs(ball_velocity.y) < ball_min_velocity) {
						ball_velocity.y = (ball_velocity.y > 0) ? ball_min_velocity : -ball_min_velocity;
					}

					if (std::abs(ball_velocity.y) > ball_max_velocity) {
						ball_velocity.y = (ball_velocity.y > 0) ? ball_max_velocity : -ball_max_velocity;
					}

					if (std::abs(ball_velocity.x) < ball_min_velocity) {
						ball_velocity.x = (ball_velocity.x > 0) ? ball_min_velocity : -ball_min_velocity;
					}

					if (std::abs(ball_velocity.x) > ball_max_velocity) {
						ball_velocity.x = (ball_velocity.x > 0) ? ball_max_velocity : -ball_max_velocity;
					}

					// Activate cooldown
					collision_cooldown = collision_threshold;
					break; //If collided with left paddle, ignore chech for right paddle
				}
			}
		}

		// Updates paddles positions
		paddle_offsets[0].y += paddle_velocity[0] * dt;
		paddle_offsets[1].y += paddle_velocity[1] * dt;

		// Updates ball position
		ball_offset.x += ball_velocity.x * dt;
		ball_offset.y += ball_velocity.y * dt;

		// *******************
		// **	GRAPHICS	**
		// *******************

		// Clear screen and set background to black
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Activates render/shader object
		SHADER.Activate();

		// Updates data in GPU
		ball_offset_vbo.Bind();
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(ball_offset), &ball_offset);
		
		paddle_offset_vbo.Bind();
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(paddle_offsets), paddle_offsets);

		// Draw the ball on screen
		ball_vao.Bind();
		glDrawElementsInstanced(GL_TRIANGLES, 3 * num_triangles, GL_UNSIGNED_INT, 0, 1);

		// Draw paddles on screen
		paddle_vao.Bind();
		glDrawElementsInstanced(GL_TRIANGLES, 3 * 2, GL_UNSIGNED_INT, 0, 2);

		// Swap frames
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Clears up everything
	ball_vao.Delete();
	ball_ebo.Delete();

	ball_position_vbo.Delete();
	ball_offset_vbo.Delete();
	ball_size_vbo.Delete();

	paddle_vao.Delete();
	paddle_ebo.Delete();
	
	paddle_position_vbo.Delete();
	paddle_offset_vbo.Delete();
	paddle_size_vbo.Delete();

	SHADER.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}