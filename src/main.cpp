#include <chip8.h>
#include <cstdint>
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include<iostream>

#include <shader.hpp>


Chip8 chip8;
constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void setupRenderQuad();
void draw(GLFWwindow *window, const Chip8 &c);





int main()
{
	// LOAD ROM
	uint8_t status = 0;
	while (!status) {
		std::cout << "ENTER ROM NAME:" << '\n';
		std::string filename;
		std::cin >> filename;
		status = chip8.load_rom("roms/" + filename + ".rom");
	}
	//******************//
	// initialize GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Chip8", NULL, NULL);
	if (window == NULL)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(window, key_callback);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	//******************//



	setupRenderQuad();


	// deltaTime
	float deltaTime = 0.0f; // Time between current frame and last frame
	float lastFrame = 0.0f; // Time of last frame
	float acc = 0.0f;
	//
	while (!glfwWindowShouldClose(window))
	{
		//glfwPollEvents();
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		acc += deltaTime;
		if (acc < (1.0f / 600.0f))
			continue;

		acc -= (1.0f / 600.0f);


		chip8.run();

		if (chip8.draw_flag)
		{
			draw(window, chip8);
			chip8.draw_flag = false;
		}
		glfwPollEvents();

	}

	glfwTerminate();
	return 0;
}







void setupRenderQuad()
{
	constexpr float vertices[] 
	{
		1.0f,  1.0f, 1.0f, 0.0f,	// top right
		1.0f, -1.0f, 1.0f, 1.0f,	// bottom right
	   -1.0f, -1.0f, 0.0f, 1.0f,	// bottom left
	   -1.0f,  1.0f, 0.0f, 0.0f		// top left 
	};


	constexpr unsigned int indices[] 
	{  
		0, 1, 3,  // first Triangle
		1, 2, 3   // second Triangle
	};

	unsigned int VAO, VBO, EBO, TEXID;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);



	glGenTextures(1, &TEXID);
	glBindTexture(GL_TEXTURE_2D, TEXID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);  
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	auto id = shader::get_shaderID("shaders/quad.vs", "shaders/quad.fs");
	glUseProgram(id);

}


void draw(GLFWwindow* window, const Chip8& c)
{
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 64, 32, 0, GL_RED, GL_UNSIGNED_BYTE, (GLvoid*)c.get_display());
	glClearColor(1, 1, 1, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glfwSwapBuffers(window);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	draw(window, chip8);
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	switch (key) {
	case GLFW_KEY_1: chip8.input[0x1] = (action) ? 1 : 0; break;
	case GLFW_KEY_2: chip8.input[0x2] = (action) ? 1 : 0; break;
	case GLFW_KEY_3: chip8.input[0x3] = (action) ? 1 : 0; break;
	case GLFW_KEY_4: chip8.input[0xC] = (action) ? 1 : 0; break;
	case GLFW_KEY_Q: chip8.input[0x4] = (action) ? 1 : 0; break;
	case GLFW_KEY_W: chip8.input[0x5] = (action) ? 1 : 0; break;
	case GLFW_KEY_E: chip8.input[0x6] = (action) ? 1 : 0; break;
	case GLFW_KEY_R: chip8.input[0xD] = (action) ? 1 : 0; break;
	case GLFW_KEY_A: chip8.input[0x7] = (action) ? 1 : 0; break;
	case GLFW_KEY_S: chip8.input[0x8] = (action) ? 1 : 0; break;
	case GLFW_KEY_D: chip8.input[0x9] = (action) ? 1 : 0; break;
	case GLFW_KEY_F: chip8.input[0xE] = (action) ? 1 : 0; break;
	case GLFW_KEY_Z: chip8.input[0xA] = (action) ? 1 : 0; break;
	case GLFW_KEY_X: chip8.input[0x0] = (action) ? 1 : 0; break;
	case GLFW_KEY_C: chip8.input[0xB] = (action) ? 1 : 0; break;
	case GLFW_KEY_V: chip8.input[0xF] = (action) ? 1 : 0; break;
	case GLFW_KEY_ESCAPE: if (action) glfwSetWindowShouldClose(window, true); break;
	}
}