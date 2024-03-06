#pragma once
#include <iostream>  
#include <OpenMesh/Core/IO/MeshIO.hh>  
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>  
#include"camera_m.h"
#include"shader_m.h"
#include<vector>
#include<float.h>
#include <algorithm>
#include <Eigen\Dense>

#define my_data_type float

typedef OpenMesh::TriMesh_ArrayKernelT<> MyMesh;
using namespace std;
#define INFINITY 1e10


// from openmesh to (V,N,F) matrices
void meshToMatrices(MyMesh mesh,
	Eigen::MatrixXf& V,	// shape:	#3 x vertices
	Eigen::MatrixXf& N,	// shape:	#3 x vertices
	Eigen::MatrixXi& F,	// shape:	#3 x faces
	Eigen::MatrixXi& E	// shape:	#2 x edges
)
{
	V.resize(3, mesh.n_vertices());
	N.resize(3, mesh.n_vertices());
	// 迭代所有顶点
	int vertexIndex = 0;
	for (MyMesh::VertexIter v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); ++v_it) {
		// 获取当前顶点的坐标并存储到矩阵中
		MyMesh::Point coord = mesh.point(*v_it);
		V(0, vertexIndex) = coord[0];
		V(1, vertexIndex) = coord[1];
		V(2, vertexIndex) = coord[2];

		MyMesh::Normal normal = mesh.normal(*v_it);
		N(0, vertexIndex) = normal[0];
		N(1, vertexIndex) = normal[1];
		N(2, vertexIndex) = normal[2];


		++vertexIndex;
	}

	F.resize(3, mesh.n_faces());
	for (MyMesh::FaceIter f_it = mesh.faces_begin();
		f_it != mesh.faces_end(); ++f_it)
	{

		MyMesh::HalfedgeHandle heh = mesh.halfedge_handle(*f_it);

		int i = 0;
		for (MyMesh::FaceHalfedgeIter fh_it = mesh.fh_iter(*f_it); fh_it.is_valid(); ++fh_it) {

			F(i++, f_it->idx()) = mesh.to_vertex_handle(*fh_it).idx();

		}
	}

	E.resize(2, mesh.n_edges());
	int edgeIndex = 0;
	for (MyMesh::EdgeIter e_it = mesh.edges_begin(); e_it != mesh.edges_end(); ++e_it) {
		MyMesh::VertexHandle vh0 = mesh.to_vertex_handle(mesh.halfedge_handle(*e_it, 0));
		MyMesh::VertexHandle vh1 = mesh.to_vertex_handle(mesh.halfedge_handle(*e_it, 1));
		E(0, edgeIndex) = vh0.idx();
		E(1, edgeIndex) = vh1.idx();
		edgeIndex++;
	}
}



// normalise model to [-0.5,0.5] cube
void normalizeModel(

	Eigen::MatrixXf& V
) {
	// 计算模型的最小和最大坐标值
	Eigen::Vector3f minCoords = V.rowwise().minCoeff();
	Eigen::Vector3f maxCoords = V.rowwise().maxCoeff();

	// 计算模型的中心点
	Eigen::Vector3f center = (minCoords + maxCoords) / 2.0;

	// 计算模型的缩放因子
	float scaleFactor = (maxCoords - minCoords).maxCoeff();

	// 对顶点坐标应用缩放和平移变换
	V = (V.colwise() - center) / scaleFactor;
}



class Model
{
public:
	MyMesh mesh;	//mesh
	Eigen::MatrixXf V;   // shape:	# 3 x vertices
	Eigen::MatrixXf N;	 // shape:	# 3 x vertices
	Eigen::MatrixXi F;	 // shape:	# 3 x faces
	Eigen::MatrixXi E;	 // shape:	# 2 x edges


private:
	unsigned int VAO;
	unsigned int VBO[2];
	unsigned int EBO;



public:
	Model(const char* filename)
	{

		mesh.request_vertex_normals();
		if (!mesh.has_vertex_normals())
		{
			std::cout << "ERROR:NO NORMALS" << std::endl;
			return;
		}
		OpenMesh::IO::Options opt;
		if (!OpenMesh::IO::read_mesh(mesh, filename))
			std::cout << "read failed" << std::endl;
		if (!opt.check(OpenMesh::IO::Options::VertexNormal))
		{
			// 通过面法线计算顶点法线
			mesh.request_face_normals();
			// mesh计算出顶点法线
			mesh.update_normals();
			// 释放面法线
			mesh.release_face_normals();
		}

		meshToMatrices(mesh, V, N, F, E);

		normalizeModel(V);

		setupBuffer();

	}

	void Draw(const Shader& shader) {
		glBindVertexArray(this->VAO);
		shader.use();
		//glDrawArrays(GL_TRIANGLES, 0, 6);
		glDrawElements(GL_TRIANGLES, F.rows() * F.cols(), GL_UNSIGNED_INT, 0);
	}

	void updateVertices(const Eigen::VectorXf& q) {
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, q.rows() * sizeof(my_data_type), q.data(), GL_STATIC_DRAW);
	}

	~Model() {
		glDeleteVertexArrays(1, &this->VAO);
		glDeleteVertexArrays(2, this->VBO);
		glDeleteVertexArrays(1, &this->EBO);
	}


private:
	void setupBuffer() {

		glGenVertexArrays(1, &this->VAO);
		glBindVertexArray(this->VAO);


		// 创建并绑定 VBO
		glGenBuffers(2, VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, V.rows() * V.cols() * sizeof(my_data_type), V.data(), GL_STATIC_DRAW);

		// 设置顶点属性指针
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(my_data_type), (void*)0);
		glEnableVertexAttribArray(0);


		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, N.rows() * N.cols() * sizeof(my_data_type), N.data(), GL_STATIC_DRAW);

		// 设置顶点属性指针
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(my_data_type), (void*)0);
		glEnableVertexAttribArray(1);


		// 创建并绑定 EBO
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, F.rows() * F.cols() * sizeof(unsigned int), F.data(), GL_STATIC_DRAW);
	}



};



