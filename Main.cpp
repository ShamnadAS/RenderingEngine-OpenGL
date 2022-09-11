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

const float DEG2RAD = 3.141593f / 180.0f;
const float RAD2DEG = 180.0f / 3.141593f;

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
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

	Vector3 cubePositions[] =
	{
		Vector3( 0.0f, 0.0f, 0.0f ),
		Vector3(-1.0f,-1.f, 1.0f ),
		Vector3( 1.0f,-0.5f, 0.5f ),
		Vector3( 1.0f, 0.5f,-1.0f ),
		Vector3( 1.5f, 0.5f,-1.5f )
	};
	Vector3 pointLightPositions[] =
	{
		Vector3( 2.0f, 2.5f, 0.5f ),
		Vector3(-2.0f, 0.5f, 1.5f ),
		Vector3( 1.5f,-3.0f,-2.5f ),
		Vector3(-3.0f,-4.0f, 3.5f )
	};
	Vector3 pointLightDiffuse[] =
	{
		Vector3( 1.5f, 0.0f, 0.0f ),
		Vector3( 0.0f, 1.5f, 0.0f ),
		Vector3( 0.0f, 0.0f, 1.5f ),
		Vector3( 1.0f, 0.0f, 1.5f )
	};


	unsigned int objectVAO, lightVAO, VBO;
	glGenVertexArrays(1, &objectVAO);
	glGenBuffers(1, &VBO);
	//VAO for object
	glBindVertexArray(objectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	
	//VAO for light
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//load texture diffuse
	unsigned int diffuseMap;
	glGenTextures(1, &diffuseMap);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);
	// set the texture wrapping/filtering options (on currently bound texture)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load and generate the texture
	int width, height, nrChannels;
	unsigned char* data = stbi_load("resources/textures/container2.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA,
			GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	//load texture specular
	unsigned int specularMap;
	glGenTextures(1, &specularMap);
	glBindTexture(GL_TEXTURE_2D, specularMap);
	// set the texture wrapping/filtering options (on currently bound texture)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load and generate the texture
	data = stbi_load("resources/textures/container2_specular.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA,
			GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	lightingShader.use();
	lightingShader.setInt("material.diffuse", 0);
	lightingShader.setInt("material.specular", 1);
	
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
		glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//light position update
		float lightSpeed = 0.1f;
		float radius = 2.0f;

		//set lighting
		lightingShader.use();
		lightingShader.setVec3("viewPos", camera.Position);
		//material
		lightingShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
		lightingShader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
		lightingShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
		lightingShader.setFloat("material.shininess", 16.0f);
		//direction light
		lightingShader.setVec3("dirLight.ambient", 0.2f, 0.2f, 0.2f);
		lightingShader.setVec3("dirLight.diffuse", 1.5f, 1.5f, 1.5f);
		lightingShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setVec3("dirLight.direction", 0.5f, -1.0f, 0.5f);
		//spotLight
		lightingShader.setVec3("spotLight.ambient", 0.2f, 0.2f, 0.2f);
		lightingShader.setVec3("spotLight.diffuse", 0.0f, 1.0f, 0.0f);
		lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("spotLight.constant", 1.0f);
		lightingShader.setFloat("spotLight.linear", 0.07f);
		lightingShader.setFloat("spotLight.quadratic", 0.017f);
		lightingShader.setFloat("spotLight.innerCutOff", cos(12.5f * DEG2RAD));
		lightingShader.setFloat("spotLight.outerCutOff", cos(17.5f * DEG2RAD));
		lightingShader.setVec3("spotLight.direction", camera.Front);
		lightingShader.setVec3("spotLight.position", camera.Position);
		//pointLight 1
		lightingShader.setVec3("pointLight[0].ambient", 0.2f, 0.2f, 0.2f);
		lightingShader.setVec3("pointLight[0].diffuse", pointLightDiffuse[0]);
		lightingShader.setVec3("pointLight[0].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLight[0].constant", 1.0f);
		lightingShader.setFloat("pointLight[0].linear", 0.07f);
		lightingShader.setFloat("pointLight[0].quadratic", 0.017f);
		lightingShader.setVec3("pointLight[0].position", pointLightPositions[0]);
		//pointLight 2
		lightingShader.setVec3("pointLight[1].ambient", 0.2f, 0.2f, 0.2f);
		lightingShader.setVec3("pointLight[1].diffuse", pointLightDiffuse[1]);
		lightingShader.setVec3("pointLight[1].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLight[1].constant", 1.0f);
		lightingShader.setFloat("pointLight[1].linear", 0.07f);
		lightingShader.setFloat("pointLight[1].quadratic", 0.017f);
		lightingShader.setVec3("pointLight[1].position", pointLightPositions[1]);
		//pointLight 3
		lightingShader.setVec3("pointLight[2].ambient", 0.2f, 0.2f, 0.2f);
		lightingShader.setVec3("pointLight[2].diffuse", pointLightDiffuse[2]);
		lightingShader.setVec3("pointLight[2].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLight[2].constant", 1.0f);
		lightingShader.setFloat("pointLight[2].linear", 0.07f);
		lightingShader.setFloat("pointLight[2].quadratic", 0.017f);
		lightingShader.setVec3("pointLight[2].position", pointLightPositions[2]);
		//pointLight 4
		lightingShader.setVec3("pointLight[3].ambient", 0.2f, 0.2f, 0.2f);
		lightingShader.setVec3("pointLight[3].diffuse", pointLightDiffuse[3]);
		lightingShader.setVec3("pointLight[3].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLight[3].constant", 1.0f);
		lightingShader.setFloat("pointLight[3].linear", 0.07f);
		lightingShader.setFloat("pointLight[3].quadratic", 0.017f);
		lightingShader.setVec3("pointLight[3].position", pointLightPositions[3]);

		//Set up transforms
		Matrix4 model;
		Matrix4 view;
		Matrix4 projection;

		//model.translate(Vector3(objectPos));
		view = camera.GetViewMatrix();
		view.translate(0.0f, 0.0f, -3.0f);
		projection = projection.perspective(camera.Zoom, (float)scr_width / (float)scr_height, 100.0f, 0.1f);

		lightingShader.setMatrix4("view", view);
		lightingShader.setMatrix4("projection", projection);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);

		glBindVertexArray(objectVAO);

		for (int i = 0; i < sizeof(cubePositions); i++) 
		{
			model.rotate(20 * i, Vector3(1.0f, 0.5f, 0.3f));
			model.translate(cubePositions[i]);
			lightingShader.setMatrix4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 64);
		}

		//set up LightSource
		model = Matrix4().identity();
		model.scale(0.2f, 0.2f, 0.2f);

		lightSourceShader.use();
		lightSourceShader.setMatrix4("view", view);
		lightSourceShader.setMatrix4("projection", projection);
		
		glBindVertexArray(lightVAO);

		for (int i = 0; i < sizeof(pointLightPositions); i++) 
		{
			model.translate(pointLightPositions[i]);
			lightSourceShader.setMatrix4("model", model);
			lightSourceShader.setVec3("diffuse", pointLightDiffuse[i]);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

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
