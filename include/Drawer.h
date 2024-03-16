#pragma once
#include"Line.h"
class Drawer {
public:
	Drawer() {}
	~Drawer() {}
public:

	void Draw(
		Eigen::MatrixXf V,   // shape:	# 3 x vertices
		Eigen::MatrixXi F,	 // shape:	# 3 x faces
		Eigen::MatrixXf N	 // shape:	# 3 x vertices
	) {

		unsigned int VAO;
		unsigned int VBO;
		unsigned int VBO_N;
		unsigned int EBO;

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		// �������� VBO
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, V.size() * sizeof(float), V.data(), GL_STATIC_DRAW);

		// ���ö�������ָ��
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);

		// �������� VBO
		glGenBuffers(1, &VBO_N);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_N);
		glBufferData(GL_ARRAY_BUFFER, N.size() * sizeof(float), N.data(), GL_STATIC_DRAW);

		// ���ö�������ָ��
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(1);


		// �������� EBO
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, F.size() * sizeof(int), F.data(), GL_STATIC_DRAW);

		glDrawElements(GL_TRIANGLES, F.size(), GL_UNSIGNED_INT, 0);
		//glDrawElements(GL_LINES, F.size(), GL_UNSIGNED_INT, 0);


		glBindVertexArray(0);

		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &VBO_N);
		glDeleteBuffers(1, &EBO);
	}

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

		// ����VAO��VBO
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);


		// ��VAO
		glBindVertexArray(VAO);

		// ��VBO���������
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(Eigen::Vector3f), points.data(), GL_STATIC_DRAW);

		// ���ö�������ָ��
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
		// ����VAO��VBO
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		// ��VAO
		glBindVertexArray(VAO);

		// ��VBO�����붥������
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Eigen::Vector3f) * 2, NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Eigen::Vector3f), &line.m_point0);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(Eigen::Vector3f), sizeof(Eigen::Vector3f), &line.m_point1);

		// ���ö�������ָ��
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// ���VAO
		glBindVertexArray(0);
	}

	void setupPointVAO(unsigned int& VAO, unsigned int& VBO, const Eigen::Point3& point) {
		// ����VAO��VBO
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		// ��VAO
		glBindVertexArray(VAO);

		// ��VBO�����붥������
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
	

		glBufferData(GL_ARRAY_BUFFER,  sizeof(Eigen::Vector3f), point.data(), GL_STATIC_DRAW);

		// ���ö�������ָ��
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// ���VAO
		glBindVertexArray(0);
	}




	


};
