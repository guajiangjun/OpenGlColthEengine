#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include"shader_m.h"

#include <Eigen\Dense>


class Line
{
public:
	unsigned int VAO;
	unsigned int VBO;
	Eigen::Vector3f m_point0;
	Eigen::Vector3f m_point1;

	Line(
		const Eigen::Vector3f& point0,
		const Eigen::Vector3f& point1
	)
		:m_point0(point0),m_point1(point1)
	{
		setupVAO();
	}
	Line() {}


	void setupVAO() {
		// 生成VAO和VBO
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		// 绑定VAO
		glBindVertexArray(VAO);

		// 绑定VBO并传入顶点数据
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Eigen::Vector3f) * 2, NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Eigen::Vector3f), &m_point0);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(Eigen::Vector3f), sizeof(Eigen::Vector3f), &m_point1);

		// 设置顶点属性指针
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// 解绑VAO
		glBindVertexArray(0);
	}

	void update() {


		// 绑定VBO并传入顶点数据
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Eigen::Vector3f), &m_point0);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(Eigen::Vector3f), sizeof(Eigen::Vector3f), &m_point1);
	}

	void draw(float lineWidth = 1.0f) {
		glBindVertexArray(VAO);

		glLineWidth(5.0f); // 设置线段宽度为5像
		glDrawArrays(GL_LINES, 0, 2);
		glLineWidth(1.0f); // 设置线段宽度为5像素
	}
	
	~Line() {
		glDeleteVertexArrays(1, &this->VAO);
		glDeleteVertexArrays(1, &this->VBO);
	}



};
