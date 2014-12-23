#include <chrono>
#include <thread>
#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <ecs/ecs.h>
#include <boost/chrono.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/smart_ptr.hpp>


#define STB_IMAGE_IMPLEMENTATION
#include <STB/stb_image.h>

#include "Texture2D.h"
#include "Batch2D.h"
#include "ThreadPool.h"

boost::mutex ioLock;

template <typename T>
struct Curve : ecs::Component<Curve<T>>{
	Curve(T prev, T next) : prev(prev), next(next){}
	T prev, next;
	template <typename U>
	T mix(const U& interpolant){
		return glm::mix(prev, next, interpolant);
	}
};

static void TestWork(int x){
	ioLock.lock();
	std::cout << boost::this_thread::get_id() << ':' << x << std::endl;
	ioLock.unlock();
}


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


	ecs::EntityManager manager;

	struct Pos : ecs::Component < Pos > {
		Pos(int x, int y) :x(x), y(y){}
		Pos(){}
		int x, y;
	};

	struct Vel : ecs::Component < Vel > {
		Vel(float f) :f(f){}
		Vel(){}
		float f;
	};

	struct A : ecs::Component < A > {
		A(char a) :a(a){}
		char a;
	};

	manager.RegisterComponent < Pos >();
	manager.RegisterComponent < Vel >();
	manager.RegisterComponent <A>();

	//ecs::Entity entity = manager.CreateEntity();
	/*
	entity.AddComponent<Pos>(3, 5);
	entity.AddComponent<A>('a');
	entity = manager.CreateEntity();
	entity.AddComponent<Pos>(4, -1);
	entity.AddComponent<Vel>(5.5f);
	entity = manager.CreateEntity();
	entity.AddComponent<Vel>(3.7f);
	entity.AddComponent<A>('b');*/


	//for (int i = 0; i < entityCount; ++i){
	//	for (ecs::Entity e : manager.GetEntities<Pos>()){
	//		std::cout << e.GetId() << " " << e.GetComponent<Pos>()->x << "," << e.GetComponent<Pos>()->y << std::endl;
	//	}

	//	for (ecs::Entity e : manager.GetEntities<Vel>()){
	//		std::cout << e.GetId() << " " << e.GetComponent<Vel>()->f << std::endl;
	//	}

	//	for (ecs::Entity e : manager.GetEntities<A>()){
	//		std::cout << e.GetId() << " " << e.GetComponent<A>()->a << std::endl;
	//	}
	//}

	using nano = boost::chrono::duration < long long, boost::nano > ;
	using timePoint = boost::chrono::high_resolution_clock::time_point;
	using highResClock = boost::chrono::high_resolution_clock;

	timePoint start, end;

	const size_t entityCount = 0;
	const size_t iterationCount = 1;
	size_t threadCount = 1;
	size_t threadMax = 1;
	size_t taskMult = 12;

	start = highResClock::now();

	std::array<Pos, entityCount> posArray;
	std::array<Vel, entityCount> velArray;

	for (int i = 0; i < entityCount; ++i){
		ecs::Entity entity = manager.CreateEntity();
		entity.AddComponent<Pos>(i, i - 1);
		entity.AddComponent<Vel>((float)i);

		Pos pos(i, i - 1);
		Vel vel((float)i);
		posArray.at(i) = pos;
		velArray.at(i) = vel;
	}

	end = highResClock::now();
	nano init = end - start;
	/*
	start = highResClock::now();

	for (int i = 0; i < iterationCount; i++){
		for (ecs::Entity e : manager.GetEntities<Pos, Vel>()){
			e.GetComponent<Pos>()->x += 4;
			e.GetComponent<Pos>()->y *= 2;
			e.GetComponent<Vel>()->f += 1;
		}
	}

	end = highResClock::now();
	nano vect = end - start;*/
	start = highResClock::now();

	auto lambdaFunc = [](Pos* position, Vel* vel)
	{
		position->x += 4;
		position->y *= 2;
		vel->f += 1;
	};

	for (int i = 0; i < iterationCount; i++){
		manager.UpdateEntities<Pos, Vel>(lambdaFunc, 0, entityCount);
	}



	end = highResClock::now();
	nano lambda = end - start;
	start = highResClock::now();

	for (int i = 0; i < iterationCount; i++){
		for (int i = 0; i < entityCount; i++){
			posArray.at(i).x += 4;
			posArray.at(i).y *= 2;
			velArray.at(i).f += 1;
		}
	}

	end = highResClock::now();
	nano arr = end - start;

	for (; threadCount < threadMax; threadCount++){
		ThreadPool threadPool(threadCount);

		auto lambdaHelp = [](ecs::EntityManager* manager, int start, int end){
			auto lambdaFunc = [](Pos* position, Vel* vel)
			{
				position->x += 4;
				position->y *= 2;
				vel->f += 1;
			};
			manager->UpdateEntities<Pos, Vel>(lambdaFunc, start, end);
		};

		start = highResClock::now();

		for (int i = 0; i < threadCount; i++){

			int entitiesPerThread = entityCount / threadCount;
			int start = i * entitiesPerThread;
			int end = (i == threadCount - 1) ? entityCount : start + entitiesPerThread;

			threadPool.AddBatchTask(boost::bind<void>(lambdaHelp, &manager, start, end));
		}

		threadPool.WaitBatch();

		end = highResClock::now();
		nano thread = end - start;

		std::cout << "count:" << threadCount << " " << thread.count() << std::endl;
	}


	ThreadPool threads;
	boost::asio::ip::tcp::socket sock(threads.ioService);
	boost::asio::ip::tcp::resolver resolver(threads.ioService);
	boost::asio::ip::tcp::resolver::query query(
		"127.0.0.1",
		"7777");
	boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);
	boost::asio::ip::tcp::endpoint endpoint = *iterator;

	std::cout << "connection to " << endpoint << std::endl;
	sock.connect(endpoint);
	std::string test("test send");
	char* buff = new char[60];
	test.copy(buff, test.length());
	int amount = sock.write_some(boost::asio::buffer(buff, 60));
	//sock.async_connect(endpoint, boost::bind<void>([](){std::cout << "TestConnect" << std::endl; }));

	int i = 0;
	double d = 0;
	for (ecs::Entity e : manager.GetEntities<Pos, Vel>()){
		d += posArray[i].x + posArray[i].y + velArray[i].f;
		d += e.GetComponent<Pos>()->x + e.GetComponent<Pos>()->y + e.GetComponent<Vel>()->f;
		i++;
	}

	std::cout << d << std::endl;
	std::cout << init.count() << std::endl;
	std::cout << lambda.count() << std::endl;
	std::cout << arr.count() << std::endl;

	char a;
	std::cin >> a;

	/*
	
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
		LOG("Glew", std::string((char*)glewGetErrorString(status)));
		fprintf(stderr, "%s\n", glewGetErrorString(status));
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);
	Texture2D smile(R"(D:\git\Augment\Augment\res\smile.png)");
	Texture2D square(R"(D:\git\Augment\Augment\res\red.png)");

	struct CTex2D : ecs::Component<CTex2D>{
		CTex2D(Texture2D* texture) :texture(texture){}
		Texture2D* texture;
	};

	ecs::EntityManager manager;
	manager.RegisterComponent<Curve<glm::vec2>>();
	manager.RegisterComponent<CTex2D>();
	
	ecs::Entity entity = manager.CreateEntity();
	entity.AddComponent<Curve<glm::vec2>>(glm::vec2(0, 0), glm::vec2(800, 800));
	entity.AddComponent<CTex2D>(&smile);

	entity = manager.CreateEntity();
	entity.AddComponent<Curve<glm::vec2>>(glm::vec2(0, 800), glm::vec2(800, 0));
	entity.AddComponent<CTex2D>(&square);

	Batch2D batch;

	glfwSwapInterval(0);
	double oldTime = 0;
	float rot = 0;
	while (!glfwWindowShouldClose(window))
	{
		LOG("main", "loop begin");

		typedef boost::chrono::duration<long, boost::nano> nano;

		boost::chrono::high_resolution_clock::time_point start = boost::chrono::high_resolution_clock::now();


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
			batch.Draw(e.GetComponent<Curve<glm::vec2>>()->mix((float)sin(currentTime) * 0.5f + 0.5f), glm::vec3(0, 0, 0), glm::vec2(.2f, .2f));
		}

		batch.End();
		glfwSwapBuffers(window);
		glfwPollEvents();
		rot = glm::mod(rot + 10.f, 360.f);


		std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 2));

		nano ugh = boost::chrono::high_resolution_clock::now() - start;

		std::cout << ugh.count() << std::endl;
	}

	//glDeleteVertexArrays(1, &vao);

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
	*/
}