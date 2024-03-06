#pragma once
#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader_m.h"

#include <iostream>
#include<string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include"camera_m.h"
#include"cube.h"
#include"mesh_m.h"
#include"Line.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void processInputForView(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);







// settings
unsigned int SCR_WIDTH = 1800;
unsigned int SCR_HEIGHT = 1600;


// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame





bool mode_view = true;

// matrices for simluation
#include"EigenTypes.h"
#include"util_simulation.h"
#include"cloth_sheet.h"



bool simulating = true;

Cloth cloth;

Eigen::VectorXf force;
Eigen::SparseMatrixf stiffness;

Eigen::MatrixXi E;

Eigen::VectorXf l0; //original length of all edges in the mesh

Eigen::VectorXf q;
Eigen::VectorXf qdot;
Eigen::SparseMatrixf M;
Eigen::SparseMatrixf P; //fixed point constraints 
Eigen::VectorXf x0; //fixed point constraints 

float dt = 1e-2; //time step
float k = 1e4;
float m = 1.0f;

#include"ground.h"
float ground_y = -0.9;
float ground_size = 1.0;
float COR = 0.2;// Coefficient of Restitution

void simulate() {
	
	while (true) {
		if (!simulating) {
			continue;
		}
	
			
			assemble_forces(force, P.transpose() * q + x0, E, l0, m, k);

			force = P * force;

			assemble_stiffness(stiffness, P.transpose() * q + x0, E, l0, k);
			stiffness = P * stiffness * P.transpose();



			Eigen::VectorXf b;
			Eigen::SparseMatrixf A;

			A = (M - dt * dt * stiffness);
			b = M * qdot + dt * force;
			Eigen::SimplicialLDLT<Eigen::SparseMatrix<float>> solver;
			solver.compute(A);
			qdot = solver.solve(b);
			
			q = q + dt * qdot;


			for (int i = 0; i < q.rows(); i += 3) {
			
				float x = q[i];
				float y = q[i + 1];
				float z = q[i + 2];

				// 检查顶点是否低于地面高度
				if (y < ground_y) {
					// 将顶点位置设置为地面高度
					q[i + 1] = ground_y;

					// 反转速度方向
					qdot[i + 1] = -qdot[i + 1] * COR;
				}
			}

		

			
	
	
	}
}




int main_simulation2()
{

	//////////////////////////////////////////////////////////////
	// glfw and glad init
	//////////////////////////////////////////////////////////////
	using namespace std;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Simulation", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	glfwSwapInterval(0); // 0 means vsync is disabled (free the limit of fps due to screen)


	//////////////////////////////////////////////////////////////
	// shader and mesh
	//////////////////////////////////////////////////////////////

	Shader pureColorShader("pure_color.vert", "pure_color.frag");

	cloth = Cloth(30);

	q.resize(cloth.num_nodes*3);
	for (int i = 0;i < q.rows() / 3;i++) {
		q.segment<3>(3 * i) = cloth.points[i];
	}

	E = cloth.E;

	l0.resize(E.rows());
	for (int i = 0;i < l0.rows();i++) {
		l0(i) = (q.segment<3>(3 * E(i, 0)) - q.segment<3>(3 * E(i, 1))).norm();
	}

	// rotate the cloth to be parallel to xz plane as initialisation
	//Eigen::AngleAxisf rotation(M_PI / 2, Eigen::Vector3f(1, 0, 0));
	//Eigen::Matrix3f transformMat = rotation.toRotationMatrix();
	//for (int i = 0;i < q.rows() / 3;i++) {
	//	q.segment<3>(3 * i) = transformMat * q.segment<3>(3 * i);
	//}


	//cout << "q:" << endl;
	//printQ(q);

	//cout << "fixed points:" << endl;
	//for (int i = 0;i < cloth.fixed_point_indices.size();i++) {
	//	cout << cloth.fixed_point_indices[i] << "\t:";
	//	cout << q.segment<3>(3 * cloth.fixed_point_indices[i]).transpose() << endl;
	//}

	// setup qdot
	qdot.resize(q.rows());
	qdot.setZero();

	// mass_matrix_particles(M, q, m);
	M.resize(q.rows(), q.rows());
	M.setIdentity();
	M *= m;

	// setup constraint matrix
	fixed_point_constraints(P, q.rows(), cloth.fixed_point_indices); // produce matrix P

	x0 = q - P.transpose() * P * q; //vector x0 contains position of all fixed nodes, zero for everything else

	// correct M, q and qdot so they are the right size
	q = P * q;
	qdot = P * qdot;
	M = P * M * P.transpose();

	//cout << "x0:" << endl;
	//printQ(x0);	









	// set up variables for render loop
	glm::mat4 view(1.0f);
	glm::mat4 model(1.0f);
	glm::mat4 projection(1.0f);
	float lastFrameTime = glfwGetTime();
	float lastFpsRecordTime = glfwGetTime();
	int frameCount = 0;

	//run simulation in seperate thread to avoid slowing down the UI
	std::thread simulation_thread(simulate);
	simulation_thread.detach();
	

	Ground ground(ground_size,ground_y);
	while (!glfwWindowShouldClose(window))
	{
		frameCount++;

		// compute deltaTime for camera smooth view move 
		float currentFrameTime = glfwGetTime();
		deltaTime = currentFrameTime - lastFrameTime;
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




		processInput(window);
		if (mode_view) {
			view = camera.GetViewMatrix();
		}



		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		model = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		pureColorShader.use();

		pureColorShader.setMat4("model", model);
		pureColorShader.setMat4("view", view);
		pureColorShader.setMat4("projection", projection);
	


		pureColorShader.setVec3("aColor", 0.2f, 0.3f, 1.0f);
		pureColorShader.setVec3("aColor", 0.502, 0.502, 0.502);
		ground.draw();


		cloth.updateVertices(P.transpose() * q + x0);
		
		
		pureColorShader.setVec3("aColor", 1.0f, 0.5f, 0.2f);
		cloth.Draw(3.0f);




		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}


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

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
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
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_RELEASE) return; //only handle press events
	if (key == GLFW_KEY_SPACE) {
		mode_view = !mode_view;
	}
	if (key == GLFW_KEY_R) {
		qdot.setZero();
		
		Eigen::VectorXf qq;
		qq.resize(cloth.points.size() * 3);
		for (int i = 0;i < qq.rows() / 3;i++) {
			qq.segment<3>(3 * i) = cloth.initial_points[i];
		}
		q = P * qq;
		
		
	}
	if (key == GLFW_KEY_F) {
		simulating = !simulating;
	}
}




