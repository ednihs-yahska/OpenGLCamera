#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <windows.h>
#include <fstream>
#include <string>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "stb_image.h"

extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

static void GLClearError() {
	while (glGetError() != GL_NO_ERROR);
}

static void GLCheckError() {
	while (GLenum error = glGetError()) {
		std::cout << "[OpenGL Error] (" << error << ")" << std::endl;
	}
}

const unsigned int WIN_WIDTH = 1200;
const unsigned int WIN_HEIGHT = 800;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = WIN_WIDTH / 2.0f;
float lastY = WIN_HEIGHT / 2.0f;

bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = ypos - lastY; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
	//std::cout << "Mouse Called" << std::endl;
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	
	camera.ProcessMouseScroll(yoffset);
}

int main(void) {
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		std::cout << "GLEW INIT ERROR" << std::endl;
	}

	glFrontFace(GL_CCW);
	int nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	std::cout << glGetString(GL_VERSION) << std::endl;
	std::cout << glGetString(GL_VENDOR) << " " << glGetString(GL_RENDERER) << std::endl;

	std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	float time = glfwGetTime();

	unsigned int container = loadTexture("res/container.jpg");
	unsigned int wall = loadTexture("res/wall.jpg");

	float vertices[] = {
		// positions                    // texture coords
		 0.5f,  0.5f, 0.0f,     1.0f, 0.0f,
		-0.5f,  0.5f, 0.0f,      0.0f, 0.0f,
		-0.5f, -0.5f, 0.0f,      0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f,      1.0f, 1.0f,
		 0.5f,  -0.5f, -1.0f,      1.0f, 1.0f,
		 0.5f, 0.5f, -1.0f,      0.0f, 0.0f,
		-0.5f, 0.5f, -1.0f,      0.0f, 0.0f,
		-0.5f, -0.5f, -1.0f,      1.0f, 0.0f
	};

	unsigned int indices[] = {
	0, 1, 3, // first triangle
	1, 2, 3,  // second triangle
	0, 4, 5, //
	0, 3, 4, //
	6, 5, 7, //
	5, 4, 7, //
	1, 6, 2, //
	6, 7, 2, //
	0, 3, 4, //
	5, 6, 0, //
	6, 1, 0, //
	3, 2, 7,
	4, 3, 7,
	};

	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glm::vec3 positions[] = { glm::vec3(0.0, 0.0, 0.0), glm::vec3(-2.0, 0.0, -4.0),  glm::vec3(2.0, 0.0, -8.0) };

	// framebuffer configuration
// -------------------------

	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	// create a color attachment texture
	unsigned int textureColorbuffer;
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIN_WIDTH, WIN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
	// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIN_WIDTH, WIN_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
// ---------------------------------

	unsigned int depthMappingbuffer;
	glGenFramebuffers(1, &depthMappingbuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMappingbuffer);
	// create a color attachment texture
	unsigned int textureMappingBuffer;
	glGenTextures(1, &textureMappingBuffer);
	glBindTexture(GL_TEXTURE_2D, textureMappingBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIN_WIDTH, WIN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureMappingBuffer, 0);
	// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	unsigned int rbo3;
	glGenRenderbuffers(1, &rbo3);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo3);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIN_WIDTH, WIN_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo3); // now actually attach it
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// ---------------------------------

	unsigned int blurFramebuffer;
	glGenFramebuffers(1, &blurFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, blurFramebuffer);
	// create a color attachment texture
	unsigned int textureBlurbuffer;
	glGenTextures(1, &textureBlurbuffer);
	glBindTexture(GL_TEXTURE_2D, textureBlurbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIN_WIDTH, WIN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureBlurbuffer, 0);
	// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	unsigned int rbo2;
	glGenRenderbuffers(1, &rbo2);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo2);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIN_WIDTH, WIN_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo2); // now actually attach it
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//---------------------------


	unsigned int downScaleBuffer;
	glGenFramebuffers(1, &downScaleBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, downScaleBuffer);
	// create a color attachment texture
	unsigned int textureDownbuffer;
	glGenTextures(1, &textureDownbuffer);
	glBindTexture(GL_TEXTURE_2D, textureDownbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIN_WIDTH, WIN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureDownbuffer, 0);
	// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	unsigned int rbo4;
	glGenRenderbuffers(1, &rbo4);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo4);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIN_WIDTH, WIN_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo4); // now actually attach it
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;


	//---------------------------
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//--------------

	float screenVertices[] = {
		// positions        // texture coords
		-1.0, -1.0, 0.0,	0.0, 1.0,
		 1.0, -1.0, 0.0,    1.0, 1.0,
		 1.0,  1.0, 0.0,    1.0, 0.0,
		-1.0,  1.0, 0.0,    0.0, 0.0
	};

	unsigned int screenIndices[] = {
		0, 1, 2, // first triangle
		2, 3, 0  // second triangle
	};

	unsigned int screenVao;
	glGenVertexArrays(1, &screenVao);
	glBindVertexArray(screenVao);

	unsigned int screenBuffer;
	glGenBuffers(1, &screenBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, screenBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(screenVertices), screenVertices, GL_STATIC_DRAW);

	unsigned int screenEBO;
	glGenBuffers(1, &screenEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, screenEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(screenIndices), screenIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	Shader shaders("res/shaders/vertex.vs", "res/shaders/fragment.fs");
	shaders.use();

	Shader screenShaders("res/shaders/screenVertex.vs", "res/shaders/screenFragment.fs");
	screenShaders.use();

	Shader blurShaders("res/shaders/blurVertex.vs", "res/shaders/blurFragment.fs");
	blurShaders.use();

	Shader depthMappingShaders("res/shaders/depthMapping.vs", "res/shaders/depthMapping.fs");
	depthMappingShaders.use();

	Shader unitShaders("res/shaders/unitShader.vs", "res/shaders/unitShader.fs");
	unitShaders.use();

	glm::mat4x4 previousProjectionMatrix = glm::mat4(1.0f);


	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		glBindVertexArray(vao);
		shaders.use();
		shaders.setInt("modelTexture", 0);
		glBindTexture(GL_TEXTURE_2D, wall);
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 100.0f);

		int uModel = glGetUniformLocation(shaders.ID, "model");
		glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(model));

		int uView = glGetUniformLocation(shaders.ID, "view");
		glUniformMatrix4fv(uView, 1, GL_FALSE, glm::value_ptr(view));

		int uProjection = glGetUniformLocation(shaders.ID, "projection");
		glUniformMatrix4fv(uProjection, 1, GL_FALSE, glm::value_ptr(projection));

		for (int p = 0; p < 3; p++) {
			glm::mat4 trans = glm::mat4(1.0f);
			trans = glm::translate(trans, positions[p]);
			int uTransform = glGetUniformLocation(shaders.ID, "uTransform");
			glUniformMatrix4fv(uTransform, 1, GL_FALSE, glm::value_ptr(trans));
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}
		glBindVertexArray(0);
		GLCheckError();
		//-----------Pass 2 ---------
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);

		screenShaders.use();
		glDisable(GL_DEPTH_TEST);
		glBindVertexArray(screenVao);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
		screenShaders.setInt("screenTexture", 1);

		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 4);

		glm::mat4x4 inverseViewProjection = glm::inverse(projection * view);

		int uInverseViewProjection = glGetUniformLocation(screenShaders.ID, "uInverseViewProjection");
		glUniformMatrix4fv(uInverseViewProjection, 1, GL_FALSE, glm::value_ptr(inverseViewProjection));

		int uPreviousProjectionM = glGetUniformLocation(screenShaders.ID, "uPreviousProjectionM");
		glUniformMatrix4fv(uPreviousProjectionM, 1, GL_FALSE, glm::value_ptr(previousProjectionMatrix));

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		GLCheckError();
		//--------------Pass 3---------DownScaling and Blurring
		/*
		glBindFramebuffer(GL_FRAMEBUFFER, downScaleBuffer);
		glClear(GL_COLOR_BUFFER_BIT);

		blurShaders.use();
		glDisable(GL_DEPTH_TEST);
		glBindVertexArray(screenVao);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, textureBlurbuffer);
		blurShaders.setInt("screenTexture", 2);

		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 4);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		//-----------------------------------
		
		glBindFramebuffer(GL_FRAMEBUFFER, depthMappingbuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		//depthMappingShaders.use();
		glBindVertexArray(vao);

		unitShaders.use();
		

		uModel = glGetUniformLocation(unitShaders.ID, "model");
		glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(model));

		uView = glGetUniformLocation(unitShaders.ID, "view");
		glUniformMatrix4fv(uView, 1, GL_FALSE, glm::value_ptr(view));

		uProjection = glGetUniformLocation(unitShaders.ID, "projection");
		glUniformMatrix4fv(uProjection, 1, GL_FALSE, glm::value_ptr(projection));
		
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, wall);
		unitShaders.setInt("modelTexture", 3);

		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 4);

		for (int p = 0; p < 3; p++) {
			glm::mat4 trans = glm::mat4(1.0f);
			trans = glm::translate(trans, positions[p]);
			int uTransform = glGetUniformLocation(unitShaders.ID, "uTransform");
			glUniformMatrix4fv(uTransform, 1, GL_FALSE, glm::value_ptr(trans));
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}
		glBindVertexArray(0);
		//-----------------------------------

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		depthMappingShaders.use();


		glDisable(GL_DEPTH_TEST);
		glBindVertexArray(screenVao);

		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, textureMappingBuffer);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, textureDownbuffer);

		depthMappingShaders.setInt("tSource", 5);
		depthMappingShaders.setInt("tSourceLow", 6);

		//glGenerateMipmap(GL_TEXTURE_2D);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 4);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	
		//-----------------------------------
		*/
		previousProjectionMatrix = projection * view * model;
		GLCheckError();
		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}
	glDeleteFramebuffers(1, &framebuffer);
	glDeleteVertexArrays(1, &screenVao);
	glDeleteBuffers(1, &screenBuffer);
	glDeleteBuffers(1, &screenEBO);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &buffer);
	glDeleteBuffers(1, &EBO);
	glfwTerminate();
	return 0;
}