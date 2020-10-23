#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"

#include <iostream>
#include <string>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow* window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

struct WallRotation
{
	float angle;
	glm::vec3 rotationVector;
};

// settings
const unsigned int SCREEN_WIDTH = 1600;
const unsigned int SCREEN_HEIGHT = 900;

// camera
Camera camera(glm::vec3(0.0f, 2.0f, 7.0f));
float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to load GLAD" << std::endl;
		return -1;
	}

	// global opengl configuration
	glEnable(GL_DEPTH_TEST);

	// shaders
	std::string shaderPath = "C:/Users/cagat/projects/fps-demo/fps-demo/shaders/";
	Shader basicShader((shaderPath + "basic.vert").c_str(), (shaderPath + "basic.frag").c_str());

	float floorVertices[] = {
		-10.0f, 0.0f, -10.0f, 0.0f, 0.0f,
		 10.0f, 0.0f, -10.0f, 5.0f, 0.0f,
		 10.0f, 0.0f,  10.0f, 5.0f, 5.0f,
		 10.0f, 0.0f,  10.0f, 5.0f, 5.0f,
		-10.0f, 0.0f,  10.0f, 0.0f, 5.0f,
		-10.0f, 0.0f, -10.0f, 0.0f, 0.0f
	};
	glm::vec3 floorPosition = glm::vec3(0.0f, 0.0f, 0.0f);

	float wallVertices[] = {
		-10.0f,  0.0f,  0.0f, 0.0f, 0.0f,
		 10.0f,  0.0f,  0.0f, 5.0f, 0.0f,
		 10.0f,  4.0f,  0.0f, 5.0f, 1.0f,
		 10.0f,  4.0f,  0.0f, 5.0f, 1.0f,
		-10.0f,  4.0f,  0.0f, 0.0f, 1.0f,
		-10.0f,  0.0f,  0.0f, 0.0f, 0.0f
	};

	glm::vec3 wallPositions[] = {
		glm::vec3(  0.0f, 0.0f, -10.0f),
		glm::vec3( 10.0f, 0.0f,   0.0f),
		glm::vec3(  0.0f, 0.0f,  10.0f),
		glm::vec3(-10.0f, 0.0f,   0.0f)
	};
	WallRotation wallRotations[4];
	wallRotations[0].angle =    0.0f;
	wallRotations[1].angle =   90.0f;
	wallRotations[2].angle =  180.0f;
	wallRotations[3].angle =  270.0f;
	wallRotations[0].rotationVector = glm::vec3(0.0f, 1.0f, 0.0f);
	wallRotations[1].rotationVector = glm::vec3(0.0f, 1.0f, 0.0f);
	wallRotations[2].rotationVector = glm::vec3(0.0f, 1.0f, 0.0f);
	wallRotations[3].rotationVector = glm::vec3(0.0f, 1.0f, 0.0f);

	// initialize objects
	unsigned int VAOs[2], VBOs[2];
	glGenVertexArrays(2, VAOs);
	glGenBuffers(2, VBOs);
	// floor
	glBindVertexArray(VAOs[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// walls
	glBindVertexArray(VAOs[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(wallVertices), wallVertices, GL_STATIC_DRAW);
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	unsigned int floorTexture, wallTexture;
	glGenTextures(1, &floorTexture);
	glBindTexture(GL_TEXTURE_2D, floorTexture);
	// wrapping/filtering options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load and generate the texture
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load("resources/textures/floor.jpg", &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		std::cout << "Failed to load texture: floor" << std::endl;
	}
	stbi_image_free(data);

	glGenTextures(1, &wallTexture);
	glBindTexture(GL_TEXTURE_2D, wallTexture);
	// wrapping/filtering options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	data = stbi_load("resources/textures/wall.jpg", &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		std::cout << "Failed to load texture: wall" << std::endl;
	}

	// Wireframe mode:
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		process_input(window);
		camera.Update(deltaTime);

		glm::vec3 rgb = glm::vec3(217.0f, 234.0f, 250.0f);
		glClearColor((rgb.r + 1.0f) / 256.0f, (rgb.g + 1.0f) / 256.0f, (rgb.b + 1.0f) / 256.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// rectangle drawing
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, floorTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, wallTexture);

		basicShader.use();
		// it is best practice to call the set the projection matrix 
		// only once outside the loop, since it rarely changes
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f);
		basicShader.setMat4("projection", projection);

		glm::mat4 view = camera.GetViewMatrix();
		basicShader.setMat4("view", view);


		glBindVertexArray(VAOs[0]);
		basicShader.setInt("ourTexture", 0);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, floorPosition);
		basicShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindVertexArray(VAOs[1]);
		basicShader.setInt("ourTexture", 1);
		for (unsigned int i = 0; i < 4; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, wallPositions[i]);
			model = glm::rotate(model, glm::radians(wallRotations[i].angle), wallRotations[i].rotationVector);
			basicShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		unsigned int error = glGetError();
		while (error)
		{
			std::cout << error << std::endl;
			error = glGetError();
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(2, VAOs);
	glDeleteBuffers(2, VBOs);
	glDeleteProgram(basicShader.ID);

	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}


/*

CONTROLS

	ESC: Exit the program
	R:   Reset camera

	Camera
	Left / Right arrows: Move the camera in the x direction
	Up / Down arrows:    Move the camera in the y direction
	Z / X:               Move the camera in the z direction


*/

void process_input(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	/*if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		theAngle -= 0.5f;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		theAngle += 0.5f;*/
	//translate_input
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS)
		camera.SetSprint(true);
	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE)
		camera.SetSprint(false);
	if (key == GLFW_KEY_RIGHT_SHIFT && action == GLFW_PRESS)
		camera.SetSprint(true);
	if (key == GLFW_KEY_RIGHT_SHIFT && action == GLFW_RELEASE)
		camera.SetSprint(false);
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		camera.Jump();
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
