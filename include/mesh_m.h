#pragma once
#include <iostream>  
#include <OpenMesh/Core/IO/MeshIO.hh>  
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>  

#include <Eigen\Dense>
#include <cmath>
#define my_data_type float

#include<omp.h>


typedef OpenMesh::TriMesh_ArrayKernelT<> MyMesh;

using namespace std;

void meshToMatrices(
	MyMesh mesh,
	Eigen::MatrixXf& V,	// shape:	#3 x vertices
	Eigen::MatrixXi& F,	// shape:	#3 x faces
	Eigen::MatrixXi& E	// shape:	#2 x edges
);


void updateNormals(
	Eigen::MatrixXf& V,	// shape:	#3 x vertices
	Eigen::MatrixXi& F,	// shape:	#3 x faces
	Eigen::MatrixXf& N	// shape:	#2 x edges
);

class triMesh
{
public:

	Eigen::MatrixXf V;   // shape:	# 3 x vertices
	Eigen::MatrixXf N;	 // shape:	# 3 x vertices
	Eigen::MatrixXi F;	 // shape:	# 3 x faces
	Eigen::MatrixXi E;	 // shape:	# 2 x edges

public:
	triMesh() {}
	triMesh(const std::string& filename)
	{
		MyMesh mesh;
		// load file
		mesh.request_vertex_normals();
		if (!mesh.has_vertex_normals())
		{
			std::cout << "ERROR:NO NORMALS" << std::endl;
			return;
		}
		OpenMesh::IO::Options opt;
		if (!OpenMesh::IO::read_mesh(mesh, filename))
			std::cout << "read failed" << std::endl;

		// normalise mesh to a unit cube centered at 0
		OpenMesh::Vec3f bbMin, bbMax;
		bbMin[0] = bbMin[1] = bbMin[2] = std::numeric_limits<float>::max();
		bbMax[0] = bbMax[1] = bbMax[2] = std::numeric_limits<float>::min();
		for (auto vit = mesh.vertices_begin(); vit != mesh.vertices_end(); ++vit)
		{
			bbMin.minimize(mesh.point(*vit));
			bbMax.maximize(mesh.point(*vit));
		}
		OpenMesh::Vec3f bbSize = bbMax - bbMin;
		float maxDim = std::max(bbSize[0], std::max(bbSize[1], bbSize[2]));
		for (auto vit = mesh.vertices_begin(); vit != mesh.vertices_end(); ++vit)
		{
			OpenMesh::Vec3f v = mesh.point(*vit);
			v -= bbMin;
			v /= maxDim;
			v -= OpenMesh::Vec3f(0.5f, 0.5f, 0.5f);
			mesh.set_point(*vit, v);
		}

		// set V,F,E and N
		meshToMatrices(mesh, V, F, E);
		update_normals();

		cout << F.cols() << "\tfaces" << endl;
		cout << V.cols() << "\tvertices" << endl;
		cout << E.cols() << "\tedges" << endl;
	}

	void update_normals() {
		updateNormals(V, F, N);
	}
	void Draw() {

		Drawer drawer;
		drawer.Draw(V, F, N);
		return;
		
	}


	~triMesh() {

	}

};
// from openmesh to (V,N,F) matrices
void meshToMatrices(
	MyMesh mesh,
	Eigen::MatrixXf& V,	// shape:	#3 x vertices
	Eigen::MatrixXi& F,	// shape:	#3 x faces
	Eigen::MatrixXi& E	// shape:	#2 x edges
)
{
	V.resize(3, mesh.n_vertices());
	F.resize(3, mesh.n_faces());
	E.resize(2, mesh.n_edges());

	// set V and N
	int vertexIndex = 0;
	for (MyMesh::VertexIter v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); ++v_it) {

		MyMesh::Point coord = mesh.point(*v_it);
		V(0, vertexIndex) = coord[0];
		V(1, vertexIndex) = coord[1];
		V(2, vertexIndex) = coord[2];

		++vertexIndex;
	}


	// set F
	int faceIndex = 0;
	for (MyMesh::FaceIter f_it = mesh.faces_begin(); f_it != mesh.faces_end(); ++f_it)
	{
		int i = 0;
		for (MyMesh::FaceVertexIter fv_it = mesh.fv_iter(*f_it); fv_it.is_valid(); ++fv_it)
		{
			F(i, faceIndex) = fv_it->idx();
			i++;
		}
		faceIndex++;
	}

	// set E
	int edgeIndex = 0;
	for (MyMesh::EdgeIter e_it = mesh.edges_begin(); e_it != mesh.edges_end(); ++e_it) {
		MyMesh::VertexHandle vh0 = mesh.to_vertex_handle(mesh.halfedge_handle(*e_it, 0));
		MyMesh::VertexHandle vh1 = mesh.to_vertex_handle(mesh.halfedge_handle(*e_it, 1));
		E(0, edgeIndex) = vh0.idx();
		E(1, edgeIndex) = vh1.idx();
		edgeIndex++;
	}
}


void updateNormals(
	Eigen::MatrixXf& V,	// shape:	#3 x vertices
	Eigen::MatrixXi& F,	// shape:	#3 x faces
	Eigen::MatrixXf& N	// shape:	#2 x edges
){
	N.setZero();

	N.resize(3, V.cols());
	N.setZero(); // 初始化法向量为0

	// 对每个面片进行遍历
	for (int i = 0; i < F.cols(); ++i) {
		// 获取当前面片的三个顶点索引
		int v1 = F(0, i);
		int v2 = F(1, i);
		int v3 = F(2, i);

		// 计算两条边向量
		Eigen::Vector3f e1 = V.col(v2) - V.col(v1);
		Eigen::Vector3f e2 = V.col(v3) - V.col(v1);

		// 计算面片的法向量
		Eigen::Vector3f normal = e1.cross(e2);
		normal.normalize(); // 归一化

		// 累加到三个顶点的法向量上
		N.col(v1) += normal;
		N.col(v2) += normal;
		N.col(v3) += normal;
	}

	// 对每个顶点的法向量进行归一化
	for (int i = 0; i < N.cols(); ++i) {
		N.col(i).normalize();
	}


}