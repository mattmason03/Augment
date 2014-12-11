#include <chrono>
#include <thread>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <ecs/ecs.h>


#define STB_IMAGE_IMPLEMENTATION
#include <STB/stb_image.h>

#include "Texture2D.h"
#include "Batch2D.h"

template <typename T>
struct Curve : ecs::Component<Curve<T>>{
	Curve(T prev, T next) : prev(prev), next(next){}
	T prev, next;
	template <typename U>
	T mix(const U& interpolant){
		return glm::mix(prev, next, interpolant);
	}
};

static void error_callback(int error, const char* description)
{
	LOG("GLFW", "Code " + error + std::string(description));
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
	window = glfwCreateWindow(480, 480, "Simple example", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);

	GLenum status = glewInit();

	if (status != GLEW_OK){
		LOG("Glew", std::string((char*)glewGetErrorString(status)));
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
	Texture2D smile(R"(C:\git\Augment\Augment\res\smile.png)");
	Texture2D square(R"(C:\git\Augment\Augment\res\red.png)");
	//Texture2D redSquare(R"(C:\Users\mamaso\Documents\Visual Studio 2013\Projects\GLFWTest\Debug\red.png)");
	//Texture2D smile(R"(C:\Users\mamaso\Documents\Visual Studio 2013\Projects\GLFWTest\Debug\smile.png)");

	struct CTex2D : ecs::Component<CTex2D>{
		CTex2D(Texture2D* texture) :texture(texture){}
		Texture2D* texture;
	};

	ecs::EntityManager manager;
	manager.RegisterComponent<Curve<glm::vec2>>();
	manager.RegisterComponent<CTex2D>();
	
	ecs::Entity entity = manager.CreateEntity();
	entity.AddComponent<Curve<glm::vec2>>(glm::vec2(0, 0), glm::vec2(400, 200));
	entity.AddComponent<CTex2D>(&smile);

	entity = manager.CreateEntity();
	entity.AddComponent<Curve<glm::vec2>>(glm::vec2(0, 400), glm::vec2(400, 0));
	entity.AddComponent<CTex2D>(&square);

	Batch2D batch;

	glfwSwapInterval(0);
	double oldTime = 0;
	float rot = 0;
	while (!glfwWindowShouldClose(window))
	{
		LOG("main", "loop begin");

		double currentTime = glfwGetTime();
		oldTime = currentTime;
		const GLfloat color[] = { (float)sin(currentTime) * 0.5f + 0.5f,
			(float)cos(currentTime) * 0.5f + 0.5f,
			0.0f, 1.0f };

		const GLfloat col[4] = { (float)sin(currentTime) * 0.5f + 0.5f, (float)sin(currentTime) * 0.5f + 0.5f, (float)sin(currentTime) * .5f + 0.5f, 1.f };

		glClearBufferfv(GL_COLOR, 0, col);

		batch.Begin();

		for (ecs::Entity e : manager.GetEntities<CTex2D, Curve<glm::vec2>>())
		{
			batch.SetTexture(*e.GetComponent<CTex2D>()->texture);
			batch.Draw(e.GetComponent<Curve<glm::vec2>>()->mix((float)sin(currentTime) * 0.5f + 0.5f), glm::vec3(0, 0, 0), glm::vec2(.1, .1));
		}

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