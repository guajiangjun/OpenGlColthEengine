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





std::vector<unsigned int> fixed_point_indices;
bool simulating = true;
unsigned int num_vertices;


Eigen::VectorXf force;
Eigen::SparseMatrixf stiffness;

Eigen::MatrixXi E;
Eigen::MatrixXf V;
Eigen::VectorXf l0; //original length of all edges in the mesh

Eigen::VectorXf q;
Eigen::VectorXf qdot;
Eigen::SparseMatrixf M;
Eigen::SparseMatrixf P; //fixed point constraints 
Eigen::VectorXf x0; //fixed point constraints 

float t = 0;
float dt = 5e-3; //time step
float k = 1e4;
float m = 1.;

void simulate() {

	while (simulating) {
		
		assemble_forces(force, P.transpose() * q + x0, E, l0, m, k);

		force = P * force;

	
		cout << "--------------------------" << endl;
		cout << "force:\t" << endl;
		cout << force << endl;
	
		//simulating = false;
		//for (int i = 0;i < force.rows() / 3;i++) {
		//	force.segment<3>(3 * i) = Eigen::Vector3f(0, -9.8f, 0.0);
		//}

		assemble_stiffness(stiffness, P.transpose() * q + x0, E, l0, k);
		stiffness = P * stiffness * P.transpose();

		cout << "--------------------------" << endl;
		cout << "stiffness:\t" << endl;
		cout << stiffness << endl;
	
		//simulating = false;
		Eigen::VectorXf b;
		Eigen::SparseMatrixf A;

		cout << "--------------------------" << endl;
		cout << "M:\t" << endl;
		cout << M << endl;
	


		A = (M - dt * dt * stiffness);
		b = M * qdot + dt * force;
		Eigen::SimplicialLDLT<Eigen::SparseMatrix<float>> solver;
		solver.compute(A);
		qdot = solver.solve(b);

		cout << "--------------------------" << endl;
		cout << "qdot:\t" << endl;
		cout << qdot << endl;
		cout << "--------------------------" << endl;

		q = q + dt * qdot;
		//qdot = qdot + dt * force;
		
		

	}
}


int main_simulation()
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
	
	
	const char* filename1 = "E:/Desktop/models/cloth/coarse_sheet.obj";
	//const char* filename1 = "E:/Desktop/models/cloth/sheet2.obj";
	//const char* filename1 = "E:/Desktop/models/cloth/sheet.obj";
	
	
	

	Model myModel(filename1);
	


	//////////////////////////////////////////////////////////////
	// simulaiton init
	//////////////////////////////////////////////////////////////

	V = myModel.V;
	E = myModel.E;

	l0.resize(E.cols());
	for (int i = 0;i < l0.rows();i++) {
		l0[i] = (V.col(E(0, i)) - V.col(E(1, i))).norm();
	}
	
	num_vertices = V.cols();
	q.resize(V.rows() * V.cols());
	q = Eigen::Map<Eigen::VectorXf>(V.data(), V.rows() * V.cols());
	// find the fixed points
	find_maxY_vertices(fixed_point_indices, V);
	//fixed_point_indices.clear();
	//fixed_point_indices.push_back(2);
	//fixed_point_indices.push_back(4);
	

	float maxY = q.segment<3>(3*fixed_point_indices[0])[1];

	// translate and rotate the cloth to be parallel to xz plane as initialisation
	Eigen::AngleAxisf rotation(M_PI / 2, Eigen::Vector3f(1, 0, 0));
	Eigen::Matrix3f transformMat = rotation.toRotationMatrix();
	for (int i = 0;i < q.rows() / 3;i++) {
		q.segment<3>(3 * i)[1] = q.segment<3>(3 * i)[1] - maxY;
		q.segment<3>(3 * i) = transformMat * q.segment<3>(3 * i);
	}


	cout << "#vertices:\t" << num_vertices << endl;
	cout << "num of fixed points:\t" << fixed_point_indices.size() << endl;

	cout << "fixed points:" << endl;
	for (int i = 0;i < fixed_point_indices.size();i++) {
		if (i > 3) {
			cout << "..." << endl;
			break;
		}
		cout << fixed_point_indices[i] << "\t:";
		cout << q.segment<3>(3 * fixed_point_indices[i]).transpose() << endl;
	}



	// setup qdot
	qdot.resize(q.rows());
	qdot.setZero();

	// mass_matrix_particles(M, q, m);
	M.resize(q.rows(), q.rows());
	M.setIdentity();
	M *= m;

	// setup constraint matrix
	fixed_point_constraints(P, q.rows(), fixed_point_indices); // produce matrix P

	x0 = q - P.transpose() * P * q; //vector x0 contains position of all fixed nodes, zero for everything else

	// correct M, q and qdot so they are the right size
	q = P * q;
	qdot = P * qdot;
	M = P * M * P.transpose();



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

	//////////////////////////////////////////////////////////////
	// render loop
	//////////////////////////////////////////////////////////////
	while (!glfwWindowShouldClose(window))
	{
		frameCount++;

		// compute deltaTime for camera smooth view move 
		float currentFrameTime = glfwGetTime();
		deltaTime = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;

		// compute fps
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


		//////////////////////////////////////////////////////////////
		// render
		//////////////////////////////////////////////////////////////
		myModel.updateVertices(P.transpose() * q + x0);

		myModel.Draw(pureColorShader);


		//////////////////////////////////////////////////////////////
		// glfw poll events
		//////////////////////////////////////////////////////////////

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
}



