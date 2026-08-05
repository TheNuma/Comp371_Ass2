#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

//vertex shader
const char* vertexShaderSource = R"(#version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;

    flat out vec3 ourColor;//flat colours so that every face is a unique colour
    uniform mat4 transform;

    void main() {
        gl_Position = transform * vec4(aPos, 1.0);
        ourColor = aColor;
    }
)";
// fragment shader
const char* fragmentShaderSource = R"(#version 330 core
    flat in vec3 ourColor;
    out vec4 FragColor;

    void main() {
        FragColor = vec4(ourColor, 1.0);
    }
)";

//State variables for all pyramid translations
float locationX = 0.0f;
float locationY = 0.0f;
float rotationX = 10.0f;
float rotationY = 45.0f;
float rotationZ = 0.0f;
float scale = 1.0f;
bool rotationKeyDown = false;

//Handles all inputs for translating the pyramid
void processInput(GLFWwindow* window) {
	const float d = 0.01f;
	const float s = 0.05f;

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		if (!rotationKeyDown)
			rotationZ += 30.0f;
		rotationKeyDown = true;
	}
	else {
		if (glfwGetKey(window, GLFW_KEY_Q) != GLFW_PRESS)
			rotationKeyDown = false;
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		if (!rotationKeyDown)
			rotationZ -= 30.0f;
		rotationKeyDown = true;
	}
	else {
		if (glfwGetKey(window, GLFW_KEY_E) != GLFW_PRESS)
			rotationKeyDown = false;
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		locationY += d;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		locationX -= d;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		locationY -= d;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		locationX += d;
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		scale += s;
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		scale -= s;
	}
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

int main() {
	//initialize GLFW
	if (!glfwInit()) {
		std::cout << "GLFW failed to initialize" << std::endl;
		return -1;
	}
	//create the display window
	GLFWwindow* window = glfwCreateWindow(800, 800, "Assignment 2", nullptr, nullptr);
	if (!window) {
		std::cout << "Window creation failed!" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);//tells openGL where to draw
	glfwSwapInterval(1);//enables v-sync 

	//initialize GLEW
	if (glewInit() != GLEW_OK) {
		std::cout << "GLEW failed to initialize" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);//correctly renders 3D faces that overlap

	//compile the shaders
	//vertex
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
	glCompileShader(vertexShader);
	//fragment
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
	glCompileShader(fragmentShader);
	//program
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	//cleanup individual shaders
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//setup the pyramid
	std::vector<GLfloat> pyramidVertices = {//the 5 distinc vertices
		//position , colour
		0.0f,  0.5f,  0.0f,   1.0f, 1.0f, 1.0f, //peak
		-0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 0.0f, //front left
		0.5f, -0.5f,  0.5f,   0.0f, 1.0f, 0.0f, //front right
		0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 1.0f, //back right
		-0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 0.0f  //back left
	};
	std::vector<GLuint> pyramidIndices = {//the 6 triangles of the pyramid
		1, 2, 0, //front face = white
		4, 0, 3, //back face = blue
		1, 0, 4, //left face = yellow
		3, 0, 2, //right face = green
		//base = red
		3, 2, 1,
		4, 3, 1
	};

	//generating buffers
	unsigned int VBO, VAO, EBO;//VBO=stores the 5 vertices; VAO=stores instruction on how to handle vertices; EBO=stores the 18 indices for which vetices to use to draw the triangles
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	//bind the pyramid VAO
	glBindVertexArray(VAO);
	//bind and populate the VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, pyramidVertices.size() * sizeof(GLfloat), pyramidVertices.data(), GL_STATIC_DRAW);
	//binds and populates the EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, pyramidIndices.size() * sizeof(GLuint), pyramidIndices.data(), GL_STATIC_DRAW);
	//set the size of each vertex (3 floats of position, 3 floats of colour)
	GLsizei vertexSize = 6 * sizeof(GLfloat);
	//define attribute 0 = position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, (void*)0);
	glEnableVertexAttribArray(0);
	//define attribute 1 = colour
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertexSize, (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	//unbind VAO
	glBindVertexArray(0);

	//initial pyramid transformation (so we see the base and at least 2 sides)
	glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 scaleVec = glm::vec3(1.0f, 1.0f, 1.0f);

	//render loop
	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);//a grey colour background
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//resets the background and resets the depth

		glUseProgram(shaderProgram);//turns shaders on

		//the transformation matrix
		glm::mat4 transform = glm::mat4(1.0f);
		//apply initial rotations
		transform = glm::rotate(transform, glm::radians(rotationX), glm::vec3(1.0f, 0.0f, 0.0f));
		transform = glm::rotate(transform, glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f));

		//Apply translations upon user input
		transform = glm::translate(transform, glm::vec3(locationX, locationY, 0));
		transform = glm::rotate(transform, glm::radians(rotationZ), glm::vec3(0.0f, 0.0f, 1.0f));
		transform = glm::scale(transform, glm::vec3(scale, scale, scale));
		processInput(window);

		//pass information to the GPU
		GLuint transforrmLoc = glGetUniformLocation(shaderProgram, "transform");
		glUniformMatrix4fv(transforrmLoc, 1, GL_FALSE, glm::value_ptr(transform));

		//draw the pyramid
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(pyramidIndices.size()), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		//swap the window buffers
		glfwSwapBuffers(window);
		//poll events
		glfwPollEvents();
	}
	//cleanup resources
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(shaderProgram);

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}