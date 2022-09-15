#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

GLfloat Ground[] = { 1.2f, 0.95f, 0.8f };

const float PI = (float)3.14159265359;
glm::vec3 gravity = glm::vec3(0.0f, -98.0f, 0.0f);

int LeftButtonDown = 0;
int RightButtonDown = 0;
int modelControl = 0;

// settings
const unsigned int SCR_WIDTH = 768;
const unsigned int SCR_HEIGHT = 768;

// camera
Camera camera(glm::vec3(3.0f, 2.5f, 12.0f));

//Camera camera(glm::vec3(0.0f, 1.5f, 2.5f), glm::vec3(0.0f, 1.0f, 0.0f), -90.f, -15.0f);
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// light information
glm::vec3 lightColor = glm::vec3(0.48f, 0.68f, 1.0f);
// shader
Shader* lightingShader;


// HOUSE KEEPING
void initGL(GLFWwindow** window);
void setupShader();
void destroyShader();
void createGLPrimitives();
void destroyGLPrimitives();

// CALLBACKS
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void processInput(GLFWwindow* window, int key, int scancode, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// DRAWING PARTS
void DrawCube(glm::mat4 model);
void DrawaddFace(glm::mat4 model);
void DrawSphere(glm::mat4 model);

bool hasTextures = false;

glm::vec3 Pos = glm::vec3(-7.0f, -3.0f, -6.0f);
//glm::vec3 Pos = glm::vec3(-2.0f, -1.0f, -3.0f);
glm::vec3 lastVelocity = glm::vec3(-15.0f, -1.0f, 15.0f);
glm::vec3 currentVelocity = glm::vec3(10.0f, -3.0f, -50.0f);
glm::vec3 acceleration = gravity;

int n = 0;
int windSelection, texture, setPos;
float t = 0.0f;
float tMax = 100.0f;
float rotateAngle = 0.0f;
float lastd, d, f, timeHit, radius, aSize, timeStepRemaining, timeStep, xMin, xMax, yMin, yMax, BallMass, cr, cf, h;
glm::vec3 bottomNorm = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 aVector, aNorm, collisionPosMax, collisionPosMin, lastPos, velocityMinusN, velocityMinusT, velocityPlusN, velocityPlusT, windVelocity, windForce;

void myDisplay()
{
	glm::mat4 sphere1 = glm::mat4(1.0f);
	glm::mat4 ground = glm::mat4(1.0f);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	sphere1 = glm::scale(sphere1, glm::vec3(0.7f, 0.7f, 0.7f));

	ground = glm::scale(ground, glm::vec3(3.0f, 3.0f, 3.0f));
	ground = glm::translate(ground, glm::vec3(1.0f, 0.86f, -0.3f));

	sphere1 = glm::translate(sphere1, glm::vec3(9, -1.4, 8));
	ground = glm::rotate(ground, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	//ground = glm::rotate(ground, glm::radians(n * (1.1f / PI)), glm::vec3(0.0f, 0.0f, 1.0f));

	//xMax = (-8.5f + 4.75f) * (-sin(n * (1.1f)/180)) + (-8.5f + 4.75f) * cos(n * (1.1f) / 180);
	//yMax = (-8.5f + 4.75f) * sin(n * (1.1f) / 180) + (-8.5f + 4.75f) * cos(n * (1.1f) / 180);
	//xMin = (-1.0f + 4.75f) * (-sin(n * (1.1f) / 180)) + (-1.0f + 4.75f) * cos(n * (1.1f) / 180);
	//yMin = (-1.0f + 4.75f) * sin(n * (1.1f) / 180) + (-1.0f + 4.75f) * cos(n * (1.1f) / 180);

	collisionPosMax = glm::vec3(-8.5f, -8.5f, -10.5f);
	collisionPosMin = glm::vec3(-1.0f, -1.0f, -5.3f);
	//collisionPosMax = glm::vec3(xMax - 4.75, yMax - 4.75, -10.5f);
	//collisionPosMin = glm::vec3(xMin - 4.75, yMin - 4.75, -5.3f);
	DrawCube(ground);
	DrawaddFace(ground);
	timeStepRemaining = h;
	timeStep = timeStepRemaining;

	if (texture == 1)
	{
		BallMass = 30.0f;
		cr = 0.98f;
		cf = 0.01f;
	}
	else if (texture == 2)
	{
		BallMass = 30.0f;
		cr = 0.95f;
		cf = 0.7f;
	}
	else if (texture == 3)
	{
		BallMass = 30.0f;
		cr = 0.94f;
		cf = 0.5f;
	}
	else if (texture == 4)
	{
		BallMass = 60.0f;
		cr = 0.45f;
		cf = 0.5f;
	}

	if (windSelection == 1)
	{
		windForce = glm::vec3(150.0f, 5000.0f, -0.5f);
	}
	else if (windSelection == 2)
	{
		windForce = glm::vec3(0.0f, 5000.0f, 0.0f);
	}

	if (t < tMax)
	{
		if (timeStepRemaining > 0 )
		{
			windVelocity = windForce / deltaTime;
			acceleration = gravity - ((windVelocity - (currentVelocity - lastVelocity)) * deltaTime) / BallMass;


			currentVelocity = currentVelocity + (acceleration * timeStep);
			lastVelocity = currentVelocity;
			lastPos = Pos;
			Pos = Pos + ((lastVelocity + currentVelocity) * timeStep / 2.0f);
			aVector = Pos - lastPos;
			aSize = sqrt(pow(aVector[0], 2) + pow(aVector[0], 2) + pow(aVector[0], 2));
			aNorm = glm::vec3(aVector[0] / aSize, aVector[1] / aSize, aVector[2] / aSize);
			lastd = (collisionPosMax[0] - lastPos[0]) * bottomNorm[0] + (collisionPosMax[1] - lastPos[1]) * bottomNorm[1] + (collisionPosMax[2] - lastPos[2]) * bottomNorm[2];
			d = (collisionPosMax[0] - Pos[0]) * bottomNorm[0] + (collisionPosMax[1] - Pos[1]) * bottomNorm[1] + (collisionPosMax[2] - Pos[2]) * bottomNorm[2];
			f = lastd / (lastd - d);
			timeHit = t + (f * timeStep);
			timeStep = f * timeStep;
			sphere1 = glm::translate(sphere1, glm::vec3(Pos[0], Pos[1], Pos[2]));

			if (aNorm[0] < 0 || aNorm[1] < 0 || aNorm[2] < 0)
			{

				if (Pos[0] < collisionPosMax[0])
				{
					Pos[0] = collisionPosMax[0];
					velocityMinusN = glm::vec3(((currentVelocity[0] * bottomNorm[0]) + (currentVelocity[1] * bottomNorm[1]) + (currentVelocity[2] * bottomNorm[2])) * bottomNorm[0], ((currentVelocity[0] * bottomNorm[0]) + (currentVelocity[1] * bottomNorm[1]) + (currentVelocity[2] * bottomNorm[2])) * bottomNorm[1], ((currentVelocity[0] * bottomNorm[0]) + (currentVelocity[1] * bottomNorm[1]) + (currentVelocity[2] * bottomNorm[2])) * bottomNorm[2]);
					velocityMinusT[0] = currentVelocity[0] - velocityMinusN[0];
					velocityPlusN[0] = -cr * velocityMinusN[0];
					velocityPlusT[0] = (1 - cf) * velocityMinusT[0];
					currentVelocity[0] = velocityPlusN[0] + velocityPlusT[0];


				}
				else if (Pos[0] > collisionPosMin[0])
				{
					Pos[0] = collisionPosMin[0];
					velocityMinusN = glm::vec3(((currentVelocity[0] * bottomNorm[0]) + (currentVelocity[1] * bottomNorm[1]) + (currentVelocity[2] * bottomNorm[2])) * bottomNorm[0], ((currentVelocity[0] * bottomNorm[0]) + (currentVelocity[1] * bottomNorm[1]) + (currentVelocity[2] * bottomNorm[2])) * bottomNorm[1], ((currentVelocity[0] * bottomNorm[0]) + (currentVelocity[1] * bottomNorm[1]) + (currentVelocity[2] * bottomNorm[2])) * bottomNorm[2]);
					velocityMinusT[0] = currentVelocity[0] - velocityMinusN[0];
					velocityPlusN[0] = -cr * velocityMinusN[0];
					velocityPlusT[0] = (1 - cf) * velocityMinusT[0];
					currentVelocity[0] = velocityPlusN[0] + velocityPlusT[0];
				}

				if (Pos[1] < collisionPosMax[1])
				{
					Pos[1] = collisionPosMax[1];
					velocityMinusN = glm::vec3(((currentVelocity[0] * bottomNorm[0]) + (currentVelocity[1] * bottomNorm[1]) + (currentVelocity[2] * bottomNorm[2])) * bottomNorm[0], ((currentVelocity[0] * bottomNorm[0]) + (currentVelocity[1] * bottomNorm[1]) + (currentVelocity[2] * bottomNorm[2])) * bottomNorm[1], ((currentVelocity[0] * bottomNorm[0]) + (currentVelocity[1] * bottomNorm[1]) + (currentVelocity[2] * bottomNorm[2])) * bottomNorm[2]);
					velocityMinusT[1] = currentVelocity[1] - velocityMinusN[1];
					velocityPlusN[1] = -cr * velocityMinusN[1];
					velocityPlusT[1] = (1 - cf) * velocityMinusT[1];
					currentVelocity[1] = velocityPlusN[1] + velocityPlusT[1];
				}
				else if (Pos[1] > collisionPosMin[1])
				{
					Pos[1] = collisionPosMin[1];
					velocityMinusN = glm::vec3(((currentVelocity[0] * bottomNorm[0]) + (currentVelocity[1] * bottomNorm[1]) + (currentVelocity[2] * bottomNorm[2])) * bottomNorm[0], ((currentVelocity[0] * bottomNorm[0]) + (currentVelocity[1] * bottomNorm[1]) + (currentVelocity[2] * bottomNorm[2])) * bottomNorm[1], ((currentVelocity[0] * bottomNorm[0]) + (currentVelocity[1] * bottomNorm[1]) + (currentVelocity[2] * bottomNorm[2])) * bottomNorm[2]);
					velocityMinusT[1] = currentVelocity[1] - velocityMinusN[1];
					velocityPlusN[1] = -cr * velocityMinusN[1];
					velocityPlusT[1] = (1 - cf) * velocityMinusT[1];
					currentVelocity[1] = velocityPlusN[1] + velocityPlusT[1];
				}
				if (Pos[2] < collisionPosMax[2])
				{
					Pos[2] = collisionPosMax[2];
					velocityMinusN= glm::vec3(((currentVelocity[0] * bottomNorm[0]) + (currentVelocity[1] * bottomNorm[1]) + (currentVelocity[2] * bottomNorm[2])) * bottomNorm[0], ((currentVelocity[0] * bottomNorm[0]) + (currentVelocity[1] * bottomNorm[1]) + (currentVelocity[2] * bottomNorm[2])) * bottomNorm[1], ((currentVelocity[0] * bottomNorm[0]) + (currentVelocity[1] * bottomNorm[1]) + (currentVelocity[2] * bottomNorm[2])) * bottomNorm[2]);
					velocityMinusT[2] = currentVelocity[2] - velocityMinusN[2];
					velocityPlusN[2] = -cr * velocityMinusN[2];
					velocityPlusT[2] = (1 - cf) * velocityMinusT[2];
					currentVelocity[2] = velocityPlusN[2] + velocityPlusT[2];
				}
				else if (Pos[2] > collisionPosMin[2])
				{
					Pos[2] = collisionPosMin[2];
					velocityMinusN = glm::vec3(((currentVelocity[0] * bottomNorm[0]) + (currentVelocity[1] * bottomNorm[1]) + (currentVelocity[2] * bottomNorm[2])) * bottomNorm[0], ((currentVelocity[0] * bottomNorm[0]) + (currentVelocity[1] * bottomNorm[1]) + (currentVelocity[2] * bottomNorm[2])) * bottomNorm[1], ((currentVelocity[0] * bottomNorm[0]) + (currentVelocity[1] * bottomNorm[1]) + (currentVelocity[2] * bottomNorm[2])) * bottomNorm[2]);
					velocityMinusT[2] = currentVelocity[2] - velocityMinusN[2];
					velocityPlusN[2] = -cr * velocityMinusN[2];
					velocityPlusT[2] = (1 - cf) * velocityMinusT[2];
					currentVelocity[2] = velocityPlusN[2] + velocityPlusT[2];
				}

			}

			timeStepRemaining = timeStepRemaining - timeStep;
		}
		n = n + 1;
		t = n * h;
		std::cout << "timestep: " << timeStep << endl;
		std::cout << "t: " << t << endl << endl;
	}
	


	
	DrawSphere(sphere1);
}

int main()
{
	std::cout << "Select material of the ball:" << endl << "1. Pixar ball (imaginary mass)" << endl << "2. Basketball" << endl << "3. Tennis ball" << endl << "4. Wood ball" << endl;
	std::cin >> texture;
	std::cout << endl << "Set start position, velocity: " << endl << "1. default" << endl << "2. randomize" << endl << "3. type in" << endl;
	std::cin >> setPos;
	if (setPos == 2)
	{
		srand(time(0));
		Pos[0] = (rand() % 10) - 8;
		Pos[1] = (rand() % 10) - 3;
		Pos[2] = (rand() % 10) - 16;

		currentVelocity[0] = (rand() % 10) - 4;
		currentVelocity[1] = (rand() % 10) - 4;
		currentVelocity[2] = (rand() % 10) - 4;
	}
	else if (setPos == 3)
	{
		std::cout << endl << "position x: ";
		std::cin >> Pos[0];
		std::cout << "position y: ";
		std::cin >> Pos[1];
		std::cout << "position z: ";
		std::cin >> Pos[2];
		std::cout << endl << "velocity x: ";
		std::cin >> currentVelocity[0];
		std::cout << "velocity y: ";
		std::cin >> currentVelocity[1];
		std::cout << "velocity z: ";
		std::cin >> currentVelocity[2];
	}
	std::cout << endl << "Add wind:" << endl << "1. Yes" << endl << "2. No" << endl;
	std::cin >> windSelection;
	std::cout << endl << "Select h size:" ;
	std::cin >> h;

	GLFWwindow* window = NULL;

	initGL(&window);
	setupShader();
	createGLPrimitives();

	glEnable(GL_DEPTH_TEST);
	// render loop
	// -----------

	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		float currentFrame = glfwGetTime();
		//std::cout << "lastFrame: " << lastFrame << std::endl;
		//std::cout << "current Frame: " << currentFrame << std::endl;
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		
		// light properties
		lightingShader->use();
		lightingShader->setVec3("light.position", glm::vec3(2.8f, 2.2f, 3.2f));
		lightingShader->setVec3("light.direction", glm::vec3(0.0f, 0.0f, 20.5f));
		lightingShader->setVec3("viewPos", camera.Position);
		//lightingShader->setFloat("light.cutOff", glm::cos(glm::radians(20.0f)));
		//lightingShader->setFloat("light.outerCutOff", glm::cos(glm::radians(28.0f)));
		lightingShader->setFloat("activateSpotlight", false);
		lightingShader->setVec3("light.ambient", 0.7f, 0.7f, 0.7f);
		lightingShader->setVec3("light.diffuse", 1.0f, 1.0f, 1.0f);
		lightingShader->setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		lightingShader->setFloat("light.constant", 0.0f);
		lightingShader->setFloat("light.linear", 0.2f);
		lightingShader->setFloat("light.quadratic", 0.0f);

		// material properties
		lightingShader->setFloat("material.shininess", 16.0f);
		lightingShader->setFloat("material.opacity", 0.5f);

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		lightingShader->setMat4("projection", projection);
		lightingShader->setMat4("view", view);

		// render
		myDisplay();

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	destroyGLPrimitives();
	destroyShader();

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

void initGL(GLFWwindow** window)
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	// --------------------
	* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Bouncing Ball", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		exit(-1);
	}
	glfwMakeContextCurrent(*window);
	glfwSetFramebufferSizeCallback(*window, framebuffer_size_callback);
	glfwSetCursorPosCallback(*window, mouse_callback);
	glfwSetMouseButtonCallback(*window, mouse_button_callback);
	glfwSetScrollCallback(*window, scroll_callback);
	glfwSetKeyCallback(*window, processInput);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		exit(-1);
	}
}

void setupShader()
{
	// Light attributes
	lightingShader = new Shader("light_casters.vs", "light_casters.fs");
	lightingShader->use();
	lightingShader->setVec3("lightColor", lightColor);
}

void destroyShader()
{
	delete lightingShader;
}

void processInput(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	float cameraSpeed = 2.5f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = (float)xpos;
		lastY = (float)ypos;
		firstMouse = false;
	}

	float xoffset = (float)(xpos - lastX) / SCR_WIDTH;
	float yoffset = (float)(lastY - ypos) / SCR_HEIGHT;

	lastX = (float)xpos;
	lastY = (float)ypos;
	if (RightButtonDown)
	{
		camera.ProcessMouseMovement(xoffset * 200, yoffset * 200);
	}
	if (LeftButtonDown)
	{
		switch (modelControl) 
		{

		}
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		LeftButtonDown = 1;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		LeftButtonDown = 0;
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		RightButtonDown = 1;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
	{
		RightButtonDown = 0;
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

class Primitive {
public:
	Primitive() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);
	}
	~Primitive() {
		if (!ebo) glDeleteBuffers(1, &ebo);
		if (!vbo) glDeleteBuffers(1, &vbo);
		if (!VAO) glDeleteVertexArrays(1, &VAO);
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
	void Draw() {
		glBindVertexArray(VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, floorTexture);
		glDrawElements(GL_TRIANGLE_STRIP, IndexCount, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
private:
	unsigned int floorTexture;
};

class Cube : public Primitive {
public:
	Cube();
	void Draw() {
		glBindVertexArray(VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, floorTexture);
		glDrawElements(GL_TRIANGLE_STRIP, IndexCount, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
private:
	unsigned int floorTexture;
};

class addFace : public Primitive {
public:
	addFace();
	void Draw() {
		glBindVertexArray(VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, floorTexture);
		glDrawElements(GL_TRIANGLE_STRIP, IndexCount, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
private:
	unsigned int floorTexture;
};

Sphere* unitSphere;
Cube* unitCube;
addFace* unitaddFace;


void createGLPrimitives()
{
	unitSphere = new Sphere();
	unitCube = new Cube();
	unitaddFace = new addFace();


}
void destroyGLPrimitives()
{
	delete unitSphere;
	delete unitCube;
	delete unitaddFace;

}

void DrawCube(glm::mat4 model)
{
	lightingShader->use();
	lightingShader->setMat4("model", model);
	lightingShader->setVec3("ObjColor", glm::vec3(Ground[0], Ground[1], Ground[2]));
	unitCube->Draw();

}

void DrawaddFace(glm::mat4 model)
{
	lightingShader->use();
	lightingShader->setMat4("model", model);
	lightingShader->setVec3("ObjColor", glm::vec3(Ground[0], Ground[1], Ground[2]));
	unitaddFace->Draw();

}

void DrawSphere(glm::mat4 model)
{
	lightingShader->use();
	model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
	model = glm::translate(model, glm::vec3(0.0f, 20.0f, 0.0f));
	lightingShader->setMat4("model", model);
	lightingShader->setVec3("ObjColor", glm::vec3(Ground[0], Ground[1], Ground[2]));
	unitSphere->Draw();
}



unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}


Cube::Cube()
{
	float data[] = {
		// positions           // normals          // texcoords
		1, 1, 1,	0, 0, 1,	1, 0,
		-1, 1, 1,   0, 0, 1,	0, 0,
		-1,-1, 1,   0, 0, 1,	0, 1,
		1,-1, 1,	0, 0, 1,	1, 1,
					   
		1, 1, 1,    1, 0, 0,	0, 0,
		1,-1, 1,	1, 0, 0,	0, 1,
		1,-1,-1,	1, 0, 0,	1, 1,
		1, 1,-1,	1, 0, 0,	1, 0,
					   
		1, 1, 1,	0, 1, 0,	1, 1,
		1, 1,-1,	0, 1, 0,	1, 0,
		-1, 1,-1,   0, 1, 0,	0, 0,
		-1, 1, 1,	0, 1, 0,	0, 1,
					  
		-1, 1, 1,	-1, 0, 0,	1, 0,
		-1, 1,-1,	-1, 0, 0,	0, 0,
		-1,-1,-1,	-1, 0, 0,	0, 1,
		-1,-1, 1,	-1, 0, 0,	1, 1,
					  
		//-1,-1,-1,   0,-1, 0,	0, 1,
		//1,-1,-1,	0,-1, 0,	1, 1,
		//1,-1, 1,	0,-1, 0,	1, 0,
		//-1,-1, 1,	0,-1, 0,	0, 0,
					   
		//1,-1,-1,	0, 0,-1,	0, 1,
		//-1,-1,-1,	0, 0,-1,	1, 1,
		//-1, 1,-1,   0, 0,-1,	1, 0,
		//1, 1,-1,	0, 0,-1,	0, 0

	};
	unsigned int indices[] = {		0, 1, 2,   
									2, 3, 0,     
								   4, 5, 6,   
								   6, 7, 4,     
								   8, 9,10,  
								   10,11, 8,      
								  12,13,14,  
								  14,15,12,     
								  //16,17,18,  
								  //18,19,16,     
								  //20,21,22,  
								  //22,23,20,
	};

	IndexCount = sizeof(indices) / sizeof(unsigned int);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	GLsizei stride = (3 + 3 + 2) * sizeof(float);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));

	glBindVertexArray(0);

	floorTexture = loadTexture("./wood.png");
	lightingShader->use();
	lightingShader->setInt("texture1", floorTexture);

}

addFace::addFace()
{
	float data[] = {
		// positions           // normals          // texcoords

		-1,-1,-1,   0,-1, 0,	0, 1,
		1,-1,-1,	0,-1, 0,	1, 1,
		1,-1, 1,	0,-1, 0,	1, 0,
		-1,-1, 1,	0,-1, 0,	0, 0,

		1,-1,-1,	0, 0,-1,	0, 1,
		-1,-1,-1,	0, 0,-1,	1, 1,
		-1, 1,-1,   0, 0,-1,	1, 0,
		1, 1,-1,	0, 0,-1,	0, 0

	};
	unsigned int indices[] = { 
									0, 1, 2,
									2, 3, 0,
								   //4, 5, 6,
								   //6, 7, 4,
	};

	IndexCount = sizeof(indices) / sizeof(unsigned int);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	GLsizei stride = (3 + 3 + 2) * sizeof(float);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));

	glBindVertexArray(0);

	floorTexture = loadTexture("./wood.png");
	lightingShader->use();
	lightingShader->setInt("texture1", floorTexture);

}

Sphere::Sphere(int NumSegs)
{
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uv;
	std::vector<unsigned int> indices;

	const unsigned int X_SEGMENTS = NumSegs;
	const unsigned int Y_SEGMENTS = NumSegs;
	const float PI = (float)3.14159265359;

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
			uv.push_back(glm::vec2(xSegment, ySegment));
			normals.push_back(glm::vec3(xPos, yPos, zPos));
		}
	}

	bool oddRow = false;
	for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
	{
		if (!oddRow)
		{
			for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
			{
				indices.push_back(y * (X_SEGMENTS + 1) + x);
				indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
			}
		}
		else
		{
			for (int x = X_SEGMENTS; x >= 0; --x)
			{
				indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
				indices.push_back(y * (X_SEGMENTS + 1) + x);
			}
		}
		oddRow = !oddRow;
	}

	IndexCount = (unsigned int)indices.size();

	std::vector<float> data;
	for (int i = 0; i < positions.size(); ++i)
	{
		data.push_back(positions[i].x);
		data.push_back(positions[i].y);
		data.push_back(positions[i].z);
		if (normals.size() > 0)
		{
			data.push_back(normals[i].x);
			data.push_back(normals[i].y);
			data.push_back(normals[i].z);
		}
		if (uv.size() > 0)
		{
			data.push_back(uv[i].x);
			data.push_back(uv[i].y);
		}
	}

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	GLsizei stride = (3 + 3 + 2) * sizeof(float);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
	glBindVertexArray(0);
	if (texture == 1)
	{
		floorTexture = loadTexture("./luxojr.png");
	}
	else if (texture == 2)
	{
		floorTexture = loadTexture("./basketball.png");
	}
	else if (texture == 3)
	{
		floorTexture = loadTexture("./tennisball.png");
	}
	else if (texture == 4)
	{
		floorTexture = loadTexture("./wood.png");
	}
	lightingShader->use();
	lightingShader->setInt("texture1", floorTexture);
}