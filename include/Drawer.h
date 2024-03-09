#pragma once
#include"Line.h"
class Drawer {
public:
	Drawer() {}
	~Drawer() {}
public:


	// draw single line
	void Draw(const Line& line, float linewidth = 5.0f) {

		unsigned int VAO;
		unsigned int VBO;
		setupLineVAO(VAO, VBO, line);

		glBindVertexArray(VAO);

		glLineWidth(linewidth); 
		glDrawArrays(GL_LINES, 0, 2);
		glLineWidth(1.0f); 

		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
	}

	// draw single point
	void Draw(const Eigen::Point3& point, float pointsize = 5.0f) {

		unsigned int VAO;
		unsigned int VBO;
		setupPointVAO(VAO, VBO, point);

		glBindVertexArray(VAO);

		glPointSize(pointsize);
		glDrawArrays(GL_POINTS, 0, 1);
		glPointSize(1.0f);
		

		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
	}

	void Draw(const std::vector<Eigen::Point3>& points, float pointsize = 5.0f) {
		unsigned int VAO;
		unsigned int VBO;

		// 生成VAO和VBO
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);


		// 绑定VAO
		glBindVertexArray(VAO);

		// 绑定VBO并填充数据
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(Eigen::Vector3f), points.data(), GL_STATIC_DRAW);

		// 设置顶点属性指针
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glPointSize(pointsize);
		glDrawArrays(GL_POINTS, 0, points.size());
		glPointSize(1.0f);


		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
	}



	void DrawLine(const Eigen::Point3& point0, const Eigen::Point3& point1) {
		this->Draw(Line(point0, point1));
	}

private:

	void setupLineVAO(unsigned int& VAO, unsigned int& VBO, const Line& line) {
		// 生成VAO和VBO
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		// 绑定VAO
		glBindVertexArray(VAO);

		// 绑定VBO并传入顶点数据
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Eigen::Vector3f) * 2, NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Eigen::Vector3f), &line.m_point0);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(Eigen::Vector3f), sizeof(Eigen::Vector3f), &line.m_point1);

		// 设置顶点属性指针
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// 解绑VAO
		glBindVertexArray(0);
	}

	void setupPointVAO(unsigned int& VAO, unsigned int& VBO, const Eigen::Point3& point) {
		// 生成VAO和VBO
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		// 绑定VAO
		glBindVertexArray(VAO);

		// 绑定VBO并传入顶点数据
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
	

		glBufferData(GL_ARRAY_BUFFER,  sizeof(Eigen::Vector3f), point.data(), GL_STATIC_DRAW);

		// 设置顶点属性指针
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// 解绑VAO
		glBindVertexArray(0);
	}




	


};
