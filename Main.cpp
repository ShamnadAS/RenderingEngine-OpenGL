#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<shader/shader_s.h>
#include<stb/stb_image.h>
#include<cmath>
#include<math/Matrices.h>
#include<camera/camera.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

//Settings
const unsigned int scr_width = 800;
const unsigned int scr_height = 600;

//Camera
float pitch = 0.0f, yaw = 0.0f;
Camera camera = Camera();

//Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//Mouse Input
float lastX = 400, lastY = 300;
bool firstMouse = true;

//Lighting
Vector3 lightPos(0.0f, 2.0f, 2.0f);

int main()
{
	//Debug session

	//	//glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//glfw window creation
	GLFWwindow* window = glfwCreateWindow(scr_width, scr_height, "MyWindow", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW Window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//glad: Load all glfw function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//config global openGL state
	glEnable(GL_DEPTH_TEST);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//build and compile our shader program
	Shader lightingShader("shader.vertex", "shader.fragment");
	Shader lightSourceShader("shader.vertex", "shaderLightSource.fragment");

	//set up vertex data (and buffer(s)) and configure vertex attributes
	float vertices[] = {
	//Coordinates        //Normals
	-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,

	-0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
	-0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f,

	 0.5f,   0.5f,  0.5f, 1.0f, 0.0f, 0.0f,
	 0.5f,   0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
	 0.5f,  -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
	 0.5f,  -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
	 0.5f,  -0.5f,  0.5f, 1.0f, 0.0f, 0.0f,
	 0.5f,   0.5f,  0.5f, 1.0f, 0.0f, 0.0f,

	-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
	 0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f,
	-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,

	-0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f
	};

	unsigned int objectVAO, lightVAO, VBO;
	glGenVertexArrays(1, &objectVAO);
	glGenBuffers(1, &VBO); 
	//VAO for object
	glBindVertexArray(objectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	
	//VAO for light
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	lightingShader.use();
	

	//wireframe mode	
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//Render loop
	while (!glfwWindowShouldClose(window))
	{
		//per-frame time logic
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//Input
		processInput(window);

		//render
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//object attributes
		Vector3 objectPos(4.0f, 0.0f, 0.0f);

		//light position update
		float lightSpeed = 0.1f;
		float radius = 2.0f;
		//lightPos.x = objectPos.x + radius * cosf((float)glfwGetTime());
		//lightPos.z = objectPos.z + radius * sinf((float)glfwGetTime());

		//Render object
		lightingShader.use();
		unsigned int objectColorLoc = glGetUniformLocation(lightingShader.ID, "objectColor");
		unsigned int lightColorLoc = glGetUniformLocation(lightingShader.ID, "lightColor");
		unsigned int viewPosLoc = glGetUniformLocation(lightingShader.ID, "viewPos");
		unsigned int lightPosLoc = glGetUniformLocation(lightingShader.ID, "lightPos");

		glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.31f);
		glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
		glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);
		glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);

		glBindVertexArray(objectVAO);

		//Set up transforms
		Matrix4 model;
		Matrix4 view;
		Matrix4 projection;

		model.translate(Vector3(objectPos));
		view = camera.GetViewMatrix();
		projection = projection.perspective(camera.Zoom, (float)scr_width / (float)scr_height, 100.0f, 0.1f);

		unsigned int viewLoc = glGetUniformLocation(lightingShader.ID, "view");
		unsigned int modelLoc = glGetUniformLocation(lightingShader.ID, "model");
		unsigned int projectionLoc = glGetUniformLocation(lightingShader.ID, "projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view.get());
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, projection.get());
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model.get());

		glDrawArrays(GL_TRIANGLES, 0, 36);

		//Render light source
		lightSourceShader.use();
		glBindVertexArray(lightVAO);

		viewLoc = glGetUniformLocation(lightSourceShader.ID, "view");
		modelLoc = glGetUniformLocation(lightSourceShader.ID, "model");
		projectionLoc = glGetUniformLocation(lightSourceShader.ID, "projection");
		 
		model = Matrix4().identity();
		model.scale(0.25f, 0.25f, 0.25f);
		model.translate(lightPos);

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view.get());
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, projection.get());
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model.get());

		glDrawArrays(GL_TRIANGLES, 0, 36);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &objectVAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);

	//glfw: terminate, clearing all previously allocated GLFW resources
	glfwTerminate();
	return 0;

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.processKeyboard(FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.processKeyboard(BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.processKeyboard(LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.processKeyboard(RIGHT, deltaTime);
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xOffset = xpos - lastX;
	float yOffset = lastY - ypos; // reversed: y ranges bottom to top
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
