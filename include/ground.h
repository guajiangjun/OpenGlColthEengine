#pragma once

#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include <Eigen\Dense>


class Ground
{
public:
	unsigned int VAO;
	unsigned int VBO;

	float groundSize;
	float groundY;
	float groundVertices[18] = {
		-groundSize, groundY, -groundSize,
		-groundSize, groundY,  groundSize,
		 groundSize, groundY,  groundSize,
		 groundSize, groundY,  groundSize,
		 groundSize, groundY, -groundSize,
		-groundSize, groundY, -groundSize
	};

	Ground(
		float size,float y
		
	)
		:groundSize(size), groundY(y)
	{
		
	}
	Ground() {}


	void setupVAO() {
		// ����VAO��VBO
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		// ��VAO
		glBindVertexArray(VAO);

		// ��VBO�����붥������
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(groundVertices), &groundVertices, GL_STATIC_DRAW);


		// ���ö�������ָ��
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		
		
	}

	
	void draw(float lineWidth = 1.0f) {
		glBindVertexArray(VAO);

		glLineWidth(5.0f); // �����߶ο��Ϊ5��
		
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glLineWidth(1.0f); // �����߶ο��Ϊ5����
	}

	~Ground() {
		glDeleteVertexArrays(1, &this->VAO);
		glDeleteVertexArrays(1, &this->VBO);
	}



};

