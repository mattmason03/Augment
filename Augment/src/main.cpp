#include <chrono>
#include <thread>

#include <GL/glew.h>
#include <GLFW/glfw3.h>


#define STB_IMAGE_IMPLEMENTATION
#include <STB/stb_image.h>

#include "Texture2D.h"
#include "Batch2D.h"


static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}
int main(int argc, char** argv)
{

	GLFWwindow* window;
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		exit(EXIT_FAILURE);
	window = glfwCreateWindow(1000, 1000, "Simple example", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}


	glfwMakeContextCurrent(window);

	GLenum status = glewInit();

	if (status != GLEW_OK){
		fprintf(stderr, "%s\n", glewGetErrorString(status));
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);
	/*
	ShaderProgram shaderProgram;
	shaderProgram.LoadShader("textured.vert", GL_VERTEX_SHADER);
	shaderProgram.LoadShader("textured.frag", GL_FRAGMENT_SHADER);
	shaderProgram.Link();
	shaderProgram.Bind();*/
	{
		Texture2D redSquare(R"(C:\git\Augment\res\smile.png)");
	}
	Texture2D redSquare(R"(C:\git\Augment\res\smile.png)");
	//Texture2D redSquare(R"(C:\Users\mamaso\Documents\Visual Studio 2013\Projects\GLFWTest\Debug\red.png)");
	//Texture2D smile(R"(C:\Users\mamaso\Documents\Visual Studio 2013\Projects\GLFWTest\Debug\smile.png)");

	Batch2D batch;

	glfwSwapInterval(0);

	double oldTime = 0;
	float rot = 0;
	while (!glfwWindowShouldClose(window))
	{
		double currentTime = glfwGetTime();
		oldTime = currentTime;
		const GLfloat color[] = { (float)sin(currentTime) * 0.5f + 0.5f,
			(float)cos(currentTime) * 0.5f + 0.5f,
			0.0f, 1.0f };

		const GLfloat col[4] = { (float)sin(currentTime) * 0.5f + 0.5f, (float)sin(currentTime) * 0.5f + 0.5f, (float)sin(currentTime) * .5f + 0.5f, 1.f };

		glClearBufferfv(GL_COLOR, 0, col);

		batch.Begin();

		
		batch.SetTexture(redSquare, &glm::vec4(0, 0, 620, 620));
		
		batch.Draw(glm::vec2(100, 100), glm::vec3((float)sin(currentTime) * 1.57f + 1.57f, 0, 0), glm::vec2(.1, .1), glm::vec2(310, 310));
		batch.Draw(glm::vec2(200, 100), glm::vec3(0, (float)sin(currentTime) * 1.57f + 1.57f, 0), glm::vec2(.1, .1), glm::vec2(310, 310));
		batch.Draw(glm::vec2(300, 100), glm::vec3(0, 0, (float)sin(currentTime) * 1.57f + 1.57f), glm::vec2(.1, .1), glm::vec2(310, 310));
		batch.Draw(glm::vec2(400, 100), glm::vec3((float)sin(currentTime) * 1.57f + 1.57f, (float)sin(currentTime) * 1.57f + 1.57f, (float)sin(currentTime) * 1.57f + 1.57f), glm::vec2(.1, .1), glm::vec2(310, 310));
		batch.Draw(glm::vec2(500, 100), glm::vec3(0, 0, 0), glm::vec2(.1, .1), glm::vec2(310, 310));

		batch.End();
		glfwSwapBuffers(window);
		glfwPollEvents();
		rot = glm::mod(rot + 10.f, 360.f);


		std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 60));
	}

	//glDeleteVertexArrays(1, &vao);

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}