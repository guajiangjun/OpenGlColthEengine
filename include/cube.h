#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include"shader_m.h"

float cubeVertices[] = {
	// Back face
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right    
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right              
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left                
	// Front face
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right        
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left        
	// Left face
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left       
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
	// Right face
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right      
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right          
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
	// Bottom face          
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left        
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
	// Top face
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right                 
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, // bottom-left  
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f  // top-left              
};

class Cube
{
public:
	unsigned int VAO;
	unsigned int VBO;
	int num_vertices = sizeof(cubeVertices)/5;
	
	Cube() {
		unsigned int VBO;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		
		
	}
	void DrawWireFrame(const Shader& shader) {
		glBindVertexArray(this->VAO);
		shader.use();
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
		glCullFace(GL_BACK);

		glLineWidth(2.0);
		glDrawArrays(GL_TRIANGLES, 0, this->num_vertices);

		//glPointSize(20.0);
		//glDrawArrays(GL_POINTS, 0, 1);
		glDisable(GL_CULL_FACE);
	}
	void Draw(const Shader& shader) {
		glBindVertexArray(this->VAO);
		shader.use();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
		glCullFace(GL_BACK);
		glDrawArrays(GL_TRIANGLES, 0, this->num_vertices);
		glDisable(GL_CULL_FACE);
	}
	~Cube() {
		glDeleteVertexArrays(1, &this->VAO);
		glDeleteVertexArrays(1, &this->VBO);
	}



};