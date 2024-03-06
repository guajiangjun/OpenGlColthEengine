#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Eigen\Dense>
#include <iostream>  



#define my_data_type float


using namespace std;





class Cloth
{
public:

	
	// [-0.5,0.5] x [0,1] in xz plane
	 
	int N = 20;

	vector<Eigen::Vector3f> points;
	vector<Eigen::Vector3f> initial_points;

	Eigen::MatrixXi E;

	std::vector<unsigned int> fixed_point_indices;

	int num_nodes;



	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;

public:
	Cloth() {}
	Cloth(int M)
		:N(M)
	{
		for (int i = 0;i < N+1;i++) {
			for (int j = 0;j < N+1;j++) {
				
				Eigen::Vector3f point;
				
				point<< -0.5 + i * 1.0f / N, 0, j * 1.0f / N;
				
				points.push_back(point);
				
			}
		}
		initial_points = points;
		E.resize(2 * N * N + 2 * N, 2);
		int edgeIndex = 0;
		for (int i = 0;i < N;i++) {
			for (int j = 0;j < N;j++) {

				int point_index = i * (N + 1) + j;

				E(edgeIndex, 0) = point_index;
				E(edgeIndex, 1) = point_index+1;

				edgeIndex++;

				E(edgeIndex, 0) = point_index;
				E(edgeIndex, 1) = point_index + 1 + N;

				edgeIndex++;
			}

			E(edgeIndex, 0) = i * (N + 1) + N;
			E(edgeIndex, 1) = (i + 1) * (N + 1) + N;

			edgeIndex++;

			E(edgeIndex, 0) = (N + 1) * N + i;
			E(edgeIndex, 1) = (N + 1) * N + i + 1;

			edgeIndex++;
		}

		for (int i = 0;i < N+1;i++) {
			if (i % 2 == 0) {

				fixed_point_indices.push_back(i*(N+1));
			}
		}
	

		num_nodes = (N + 1) * (N + 1);

		setupBuffer();
	
	}



	void updateVertices(const Eigen::VectorXf& q) {
		// 绑定VBO并填充数据
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, q.rows()*sizeof(float), q.data(), GL_STATIC_DRAW);


		if (q.size() / 3 != points.size()) {
			cout << "Cloth::updateVertices wrong!" << endl;
		}
		for (int i = 0;i < q.size() / 3;i++) {
			points[i] = q.segment<3>(3 * i);
		}
	}
	void Draw(float lineWidth = 5.0f) {

		//for (int i = 0;i < E.rows();i++) {
			//Line(points[E(i, 0)], points[E(i, 1)]).draw();
		//}

		glLineWidth(lineWidth); // 设置线段宽度为5像
		glPointSize(lineWidth);

		glBindVertexArray(this->VAO);
		
		//glDrawArrays(GL_TRIANGLES, 0, 6);

		// 绘制点

		//glDrawArrays(GL_POINTS, 0, points.size());

			// 绘制线
		glDrawElements(GL_LINES, E.size() * 2, GL_UNSIGNED_INT, 0);


		


	}
	~Cloth() {
		//glDeleteVertexArrays(1, &this->VAO);
		//glDeleteBuffers(1, &this->VBO);
		//glDeleteBuffers(1, &this->EBO);
	}


	void setupBuffer() {

	

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);



		// 绑定VAO
		glBindVertexArray(VAO);

		// 绑定VBO并填充数据
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(Eigen::Vector3f), points.data(), GL_STATIC_DRAW);


		Eigen::MatrixXi EE;
		EE.resize(2, E.rows());
		for (int i = 0;i < E.rows();i++) {
			EE(0, i) = E(i, 0);
			EE(1, i) = E(i, 1);
		}


		// 绑定EBO并填充数据
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, E.size() * sizeof(unsigned int), EE.data(), GL_STATIC_DRAW);

		// 设置顶点属性指针
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}



};




