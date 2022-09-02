#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <learnopengl/camera.h>
#include <iostream>
#include <stdio.h>
#define STB_IMAGE_IMPLENTATION
#include <stb_image.h>

const float PI = (float)3.14159265359;

GLfloat P1[] = { 0.0f, 0.0f, 0.0f };
GLfloat P2[] = { 10.0f, 0.0f, 0.0f };
GLfloat P3[] = { 10.0f, 0.0f, 10.0f };
GLfloat P4[] = { 0.0f, 0.0f, 10.0f };
GLfloat P5[] = { 0.0f, 10.0f, 0.0f };
GLfloat P6[] = { 10.0f, 10.0f, 0.0f };
GLfloat P7[] = { 10.0f, 10.0f, 10.0f };
GLfloat P8[] = { 0.0f, 10.0f, 10.0f };

glm::vec3 v1 = {0.0f, 0.0f, 0.0f};
glm::vec3 v2 = {10.0f, 0.0f, 0.0f};
glm::vec3 v3 = {10.0f, 0.0f, 10.0f};
glm::vec3 v4 = {0.0f, 0.0f, 10.0f};
glm::vec3 v5 = {0.0f, 10.0f, 0.0f};
glm::vec3 v6 = {10.0f, 10.0f, 0.0f};
glm::vec3 v7 = {10.0f, 10.0f, 10.0f};
glm::vec3 v8 = {0.0f, 10.0f, 10.0f};

GLfloat BallColor1[] = { 1.0f, 0.0f, 0.0f };
GLfloat BallColor2[] = { 1.0f, 1.0f, 0.0f };
GLfloat BackgroundColor1[] = { 0.0f, 0.0f, 1.0f };

const unsigned int SCR_WIDTH = 960;
const unsigned int SCR_HEIGHT = 540;

Camera camera(glm::vec3(0.0f, 1.0f, 3.0f));

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

void initGL(GLFWwindow** window);
void createGLPrimitives();
void destroyGLPrimitives();

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void myDisplay()
{
	glClearColor(0.1f, 0.3f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

int main()
{
	GLFWwindow* window = NULL;
	initGL(&window);
	
	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		myDisplay();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

void initGL(GLFWwindow** window)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif 
	* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Bouncing Ball - Yeeun Park", NULL, NULL);
	if (window == NULL)
	{
		glfwTerminate();
		exit(-1);
	}
	glfwMakeContextCurrent(*window);
	glfwSetFramebufferSizeCallback(*window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		exit(-1);
	}

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

class Primitive {
public:
	Primitive() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);
	}
	~Primitive() {
		
	}
	void Draw() {
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLE_STRIP, IndexCount, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
protected:
	unsigned int VAO = 0, vbo = 0, ebo = 0;
	unsigned int IndexCount = 0;
	float height = 1.0f;
	float radius[2] = { 1.0f, 1.0f };
};
class Sphere : public Primitive {
public:
	Sphere(int NumSegs = 16);
};
Sphere* unitSphere;

void createGLPrimitives()
{
	unitSphere = new Sphere();
}

void destroyGLPrimitives()
{
	delete unitSphere;
}

Sphere::Sphere(int NumSegs)
{
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> indices;

	const unsigned int X_SEGMENTS = NumSegs;
	const unsigned int Y_SEGMENTS = NumSegs;

	for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
	{
		for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
		{
			float xSegment = (float)x / (float)X_SEGMENTS;
			float ySegment = (float)y / (float)Y_SEGMENTS;
			float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
			float yPos = std::cos(ySegment * PI);
			float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

			positions.push_back(glm::vec3(xPos, yPos, zPos));
			normals.push_back(glm::vec3(xPos, yPos, zPos));
		}
	}

	bool oddRow = false;
	for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
	{
		
	}
}