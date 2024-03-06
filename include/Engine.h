#pragma once
#include "shader_m.h"

#include <iostream>
#include<string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include"RenderContex.h"
#include<iomanip>



class Engine {
public:
	GLFWwindow* window;

	glm::mat4 view;
	glm::mat4 model;
	glm::mat4 projection;

	RenderContex renderContex;

public:
	Engine() {};
	~Engine() {};

	virtual void before_renderLoop() {};
	virtual void after_renderLoop() {};
	virtual void render_inLoop() {};
	virtual void simulate_restart_handle() {};
	virtual void simlate_update() {};
	

	void launch() {
		initial_opengl();

		model = glm::mat4(1.0f);
		view = glm::mat4(1.0f);
		projection = glm::mat4(1.0f);


		before_renderLoop();


		float lastFrameTime = glfwGetTime();
		float lastFpsRecordTime = glfwGetTime();
		int frameCount = 0;
		while (!glfwWindowShouldClose(window))
		{
			frameCount++;

			// compute deltaTime for camera smooth view move 
			float currentFrameTime = glfwGetTime();
			renderContex.deltaTime = currentFrameTime - lastFrameTime;
			lastFrameTime = currentFrameTime;

			//compute fps
			if (currentFrameTime - lastFpsRecordTime >= 1.0) {
				std::ostringstream titleStream;
				titleStream << "Simulation @ FPS: " << std::fixed << std::setprecision(1)
					<< frameCount;
				glfwSetWindowTitle(window, titleStream.str().c_str());

				frameCount = 0;
				lastFpsRecordTime = currentFrameTime;

			}




			renderContex.processInput(window);
			if (renderContex.mode_view) {
				view = renderContex.camera.GetViewMatrix();
			}



			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			model = glm::mat4(1.0f);
			projection = glm::perspective(glm::radians(renderContex.camera.Zoom), (float)renderContex.SCR_WIDTH / (float)renderContex.SCR_HEIGHT, 0.1f, 100.0f);

			render_inLoop();



			glfwSwapBuffers(window);
			glfwPollEvents();
		}
		after_renderLoop();

		end_opengl();
	}



	void initial_opengl() {
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		window = glfwCreateWindow(renderContex.SCR_WIDTH, renderContex.SCR_HEIGHT, "Simulation", NULL, NULL);
		if (window == NULL)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			exit(-1);
		}
		glfwMakeContextCurrent(window);

		glfwSetFramebufferSizeCallback(window, RenderContex::framebuffer_size_callback);
		glfwSetScrollCallback(window, RenderContex::scroll_callback);
		glfwSetKeyCallback(window, RenderContex::key_callback);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			exit(-1);
		}
		glfwSwapInterval(0); // 0 means vsync is disabled (free the limit of fps due to screen)
	}
	void end_opengl() {
		glfwTerminate();
	};
	
	void simulate() {

		while (true) {
			if (renderContex.restart) {
				simulate_restart_handle();
				renderContex.restart = false;
			}

			if (!renderContex.simulating) {
				continue;
			}

			simlate_update();
		}
	}


	




};

