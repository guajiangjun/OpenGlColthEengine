#pragma once
#include "shader_m.h"

#include <iostream>
#include<string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include"RenderContex.h"
#include<iomanip>

// imgui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include"../extern/ImGuiFileBrowser.h"

#include <thread>



class Engine {

public:
	void launch() {

		initial_opengl();

		initial_imgui(2.0f);

		initial_variables();

		before_renderLoop(); // defined by user

		if (renderContex.simulating) {
			//std::thread simulation_thread(simulate);
			std::thread simulation_thread(&Engine::simulate, this);
			simulation_thread.detach();
		}

		while (!glfwWindowShouldClose(window))
		{
			before_render();
			render(); // defined by user
			after_render();
		}
		after_renderLoop(); // defined by user
		destory_imgui();
		glfwTerminate();
	}

public:

	GLFWwindow* window;

	glm::mat4 view;
	glm::mat4 model;
	glm::mat4 projection;

	RenderContex renderContex;

	float lastFrameTime;
	float lastFpsRecordTime;
	int frameCount;
	
	imgui_addons::ImGuiFileBrowser file_dialog; // As a class member or globally

	Shader defaultPureColorShader;
	Shader defaultPhongShader;


public:
	Engine() {

	};
	~Engine() {};

	virtual void before_renderLoop() {};// defined by users
	virtual void render() {};	// defined by users
	virtual void after_renderLoop() {};	// defined by users

	virtual void simulate_restart_handle() {}; // defined by user
	virtual void simlate_update() {}; // defined by user
	
	virtual void open_file() {}; // defined by user

	




public:
	
   // Now inside any function
	void showMainMenu()
	{

		bool open = false;
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Menu"))
			{
				if (ImGui::MenuItem("Open", NULL))
					open = true;
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		//Remember the name to ImGui::OpenPopup() and showFileDialog() must be same...
		if (open)
		{
			ImGui::OpenPopup("Open File");
			renderContex.simulating = false;
		}

		/* Optional third parameter. Support opening only compressed rar/zip files.
		 * Opening any other file will show error, return false and won't close the dialog.
		 */
		if (
			file_dialog.showFileDialog
			(
				"Open File",
				imgui_addons::ImGuiFileBrowser::DialogMode::OPEN,
				ImVec2(700, 810),
				".obj"
			)
			)
		{
			
			std::cout << file_dialog.selected_fn << std::endl;      // The name of the selected file or directory in case of Select Directory dialog mode
			std::cout << file_dialog.selected_path << std::endl;    // The absolute path to the selected file

			open_file();

			renderContex.simulating = true;

		}

	}

	

public:
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

private:
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
		glfwSetCursorPosCallback(window, RenderContex::mouse_callback);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			exit(-1);
		}
		glfwSwapInterval(0); // 0 means vsync is disabled (free the limit of fps due to screen)
		glEnable(GL_DEPTH_TEST);
		
	}

	void initial_variables() {
		model = glm::mat4(1.0f);
		view = glm::mat4(1.0f);
		projection = glm::mat4(1.0f);

		lastFrameTime = glfwGetTime();
		lastFpsRecordTime = glfwGetTime();
		frameCount = 0;

		// pure color type shader 
		defaultPureColorShader = Shader(
			"shaders/pure_color.vert",
			"shaders/pure_color.frag"
		);
		defaultPureColorShader.use();
		defaultPureColorShader.setVec3("aColor", 0.502, 0.502, 0.502);

		// phong type shader 
		defaultPhongShader = Shader(
			"shaders/phong_shader.vert",
			"shaders/phong_shader.frag"
		);
		defaultPhongShader.use();
		defaultPhongShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
		defaultPhongShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		defaultPhongShader.setVec3("lightPos", 1.2f, 1.0f, 2.0f);

	

	}

	// update model,view,projection
	void update_variables() {
		model = glm::mat4(1.0f);
		if (renderContex.mode_view) {
			view = renderContex.camera.GetViewMatrix();
		}
		projection = glm::perspective(glm::radians(renderContex.camera.Zoom), (float)renderContex.SCR_WIDTH / (float)renderContex.SCR_HEIGHT, 0.1f, 100.0f);

		defaultPureColorShader.use();
		defaultPureColorShader.setMat4("model", model);
		defaultPureColorShader.setMat4("view", view);
		defaultPureColorShader.setMat4("projection", projection);


		defaultPhongShader.use();
		defaultPhongShader.setVec3("viewPos", renderContex.camera.Position);
		defaultPhongShader.setMat4("model", model);
		defaultPhongShader.setMat4("view", view);
		defaultPhongShader.setMat4("projection", projection);


	 }


	void initial_imgui(float dpi_scaling = 1.0f) {
		// Initialize ImGUI
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 330");

		ImGui::GetIO().FontGlobalScale = dpi_scaling;
		ImGui::GetStyle().ScaleAllSizes(dpi_scaling);
	}

	void initial_imgui_in_renderLoop() {
		// Tell OpenGL a new frame is about to begin
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void destory_imgui() {
		// Deletes all ImGUI instances
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void showFPS() {
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
	}


	void before_render() {
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderContex.processInput(window);

		showFPS();
		initial_imgui_in_renderLoop();

		update_variables();

		
	}

	void after_render() {
		// Renders the ImGUI elements
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

};

