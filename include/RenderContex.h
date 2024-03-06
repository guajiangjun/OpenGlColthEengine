#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include"camera_m.h"

class RenderContex {
public:
	static unsigned int SCR_WIDTH;// = 1800;
	static unsigned int SCR_HEIGHT;// = 1600;

	// camera
	static Camera camera;// = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
	static float lastX;// = SCR_WIDTH / 2.0f;
	static float lastY;// = SCR_HEIGHT / 2.0f;
	static bool firstMouse;// = true;

	// timing
	float deltaTime = 0.0f;	// time between current frame and last frame



	static bool mode_view;// = true;
	static bool simulating;// = true;
	static bool restart;// = true;

public:
	void processInput(GLFWwindow* window)
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		if (mode_view) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			processInputForView(window);
			glfwSetCursorPosCallback(window, mouse_callback);
			//view = camera.GetViewMatrix();
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			glfwSetCursorPosCallback(window, 0);
			firstMouse = true;
		}
	}


	void processInputForView(GLFWwindow* window)
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera.ProcessKeyboard(FORWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.ProcessKeyboard(BACKWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera.ProcessKeyboard(LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.ProcessKeyboard(RIGHT, deltaTime);

	}


	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (action == GLFW_RELEASE) return; //only handle press events
		if (key == GLFW_KEY_SPACE) {
			mode_view = !mode_view;
		}
		// �����ӵ��߼�
		if (key == GLFW_KEY_F) {
			simulating = !simulating; // toggle animation
		}
		// �����ӵ��߼�
		if (key == GLFW_KEY_R) {
			restart = true; // toggle animation
		}

	}

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		// make sure the viewport matches the new window dimensions; note that width and 
		// height will be significantly larger than specified on retina displays.
		SCR_WIDTH = width;
		SCR_HEIGHT = height;
		glViewport(0, 0, width, height);
	}

	// glfw: whenever the mouse moves, this callback is called
	static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
	{
		float xpos = static_cast<float>(xposIn);
		float ypos = static_cast<float>(yposIn);

		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

		lastX = xpos;
		lastY = ypos;

		camera.ProcessMouseMovement(xoffset, yoffset);
	}

	// glfw: whenever the mouse scroll wheel scrolls, this callback is called
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		camera.ProcessMouseScroll(static_cast<float>(yoffset));
	}

};

unsigned int RenderContex::SCR_WIDTH = 1800;
unsigned int RenderContex::SCR_HEIGHT = 1600;
Camera RenderContex::camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
float RenderContex::lastX = SCR_WIDTH / 2.0f;
float RenderContex::lastY = SCR_HEIGHT / 2.0f;
bool RenderContex::firstMouse = true;

bool RenderContex::mode_view = true;
bool RenderContex::simulating = true;
bool RenderContex::restart = false;



