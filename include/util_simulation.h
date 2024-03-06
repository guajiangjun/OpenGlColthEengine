#pragma once
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include "EigenTypes.h"
#include <vector>

void find_maxY_vertices(
    std::vector<unsigned int>& indices, 
    Eigen::Ref<const Eigen::MatrixXf> V, 
    float tol = 1e-3
) {
    // compare the y value
    
    float maxY = V.row(1).maxCoeff();

    cout << "maxY:\t" << maxY << endl;
    
    for (unsigned int vi = 0; vi < V.cols(); ++vi) {

        if (std::abs(V(1, vi) - maxY) <= tol) {
            
            indices.push_back(vi);
        }
    }

}




//Input:
//  q_size - total number of scalar generalized coordinates (3n for n vertices)
//  indices - m indices (row ids in V) for fixed vertices
//Output:
//  P - 3(n-m)x3n sparse matrix which projects out fixed vertices
void fixed_point_constraints(Eigen::SparseMatrixf& P, unsigned int q_size, const std::vector<unsigned int> indices) {

	P.resize(q_size - 3 * indices.size(), q_size);
	P.setZero();
	int count = 0;

	//要让P的每一行都有1，1要往后挪
	//P 1 0 0 0 0 0 0 0 0 
	//  0 1 0 0 0 0 0 0 0
	//  0 0 1 0 0 0 0 0 0
	//  0 0 0 0 0 0 1 0 0
	//  0 0 0 0 0 0 0 1 0
	//  0 0 0 0 0 0 0 0 1

	



	
	for (int i = 0; i < q_size - 3 * indices.size(); i++) {

		while (count < indices.size() && i == 3 * indices[count] - 3* count) 
		{ 
			
			count++; 
		}

		P.insert(i, i + 3 * count) = 1.0;

	}

	

	
}





//Input:
//  q - generalized coordinates for the mass-spring system
//  qdot - generalized velocity for the mass spring system
//  V - the nx3 matrix of undeformed vertex positions. Each row is a single undeformed vertex position.
//  E - the mx2 spring connectivity matrix. Each row contains to indices into V that indicate a spring between those vertices.
//  l0 - the mx1 vector of undeformed spring lengths
//  m - the mass of each particle in the mass-spring system
//  k - the stiffness of each spring in the mass-spring system
//Output:
//  f - the vector 3xn vector of forces acting on each node of the mass-spring system
void assemble_forces(
    Eigen::VectorXf& f, 
    Eigen::Ref<const Eigen::VectorXf> q, 
    Eigen::Ref<const Eigen::MatrixXi> E, 
    Eigen::Ref<const Eigen::VectorXf> l0, 
    float m,
	float k
) 
{
    f.resize(q.size());
    f.setZero();
    for (int i = 0; i < E.rows()*E.cols()/2; i++) {
        Eigen::Vector3f q0, q1;

        if (E.cols() == 2) {
            q0 = q.segment<3>(3 * E(i,0));
            q1 = q.segment<3>(3 * E(i,1));
        }
        else {
            q0 = q.segment<3>(3 * E(0, i));
            q1 = q.segment<3>(3 * E(1, i));
        }

        float l0_ = l0(i);

        
        float l = (q0 - q1).norm();
        Eigen::Vector3f f0 = k * (l - l0_) * (1 / l) * (q1 - q0);
        
        if (E.cols() == 2) {
            f.segment<3>(3 * E(i, 0)) += f0;
            f.segment<3>(3 * E(i, 1)) -= f0;
        }
        else {
            f.segment<3>(3 * E(0, i)) += f0;
            f.segment<3>(3 * E(1, i)) -= f0;
        }

    }

    for (int i = 0; i < q.size() / 3; i++) {
        f.segment<3>(3 * i) += Eigen::Vector3f(0, -9.8f, 0);
    }
    
}


void d2V_spring_particle_particle_dq2(
    Eigen::Ref<Eigen::Matrix66f> H, 
    Eigen::Ref<const Eigen::Vector3f> q0, 
    Eigen::Ref<const Eigen::Vector3f> q1, 
    float l0, 
    float stiffness
) {
    float l = (q0 - q1).norm();
    Eigen::Matrix66f BtB;
    BtB <<
        1, 0, 0, -1, 0, 0,
        0, 1, 0, 0, -1, 0,
        0, 0, 1, 0, 0, -1,
        -1, 0, 0, 1, 0, 0,
        0, -1, 0, 0, 1, 0,
        0, 0, -1, 0, 0, 1;
    Eigen::Vector6f q;
    q << q0(0),
        q0(1),
        q0(2),
        q1(0),
        q1(1),
        q1(2);
    H = -stiffness * BtB + stiffness * l0 * BtB / l - stiffness * l0 * (BtB * q * (BtB * q).transpose()) / (l * l * l);
}



//Input:
//  q - generalized coordinates for the mass-spring system
//  qdot - generalized velocity for the mass spring system
//  V - the nx3 matrix of undeformed vertex positions. Each row is a single undeformed vertex position.
//  E - the mx2 spring connectivity matrix. Each row contains to indices into V that indicate a spring between those vertices.
//  l0 - the mx1 vector of undeformed spring lengths
//  k - the stiffness of each spring in the mass-spring system
//Output:
//  K - the 3nx3n sparse stiffness matrix which is the negative hessian of the potential energy function. 
void assemble_stiffness(
    Eigen::SparseMatrixf& K,
    Eigen::Ref<const Eigen::VectorXf> q,
    Eigen::Ref<const Eigen::MatrixXi> E, 
    Eigen::Ref<const Eigen::VectorXf> l0,
    float k
) {
    typedef Eigen::Triplet<float> T;
    std::vector<T> tripletList;
    tripletList.reserve(q.rows() * q.rows());
    K.resize(q.rows(), q.rows());
    K.setZero();
    //std::cout << "ASSEMBLE_STIFFNESS::DEBUG::K.rows():" << K.rows() << std::endl;
    for (int y = 0; y < E.rows()*E.cols()/2; y++)
    {
        Eigen::Matrix66f H;
         
        int i, j;
        if (E.rows() == 2) {
            i = E(0,y);
            j = E(1,y);
        }
        else {
            i = E(y,0);
            j = E(y,1);
        }
        Eigen::Vector3f q0, q1;
        
        q0 << q(i * 3), q(i * 3 + 1), q(i * 3 + 2);
        q1 << q(j * 3), q(j * 3 + 1), q(j * 3 + 2);
        d2V_spring_particle_particle_dq2(H, q0, q1, l0(y), k);
        for (int m = 0; m < 3; m++) {
            for (int n = 0; n < 3; n++) {
                tripletList.push_back(T(3 * i + m, 3 * i + n, H(m, n)));
                tripletList.push_back(T(3 * i + m, 3 * j + n, H(m, n + 3)));
                tripletList.push_back(T(3 * j + m, 3 * i + n, H(m + 3, n)));
                tripletList.push_back(T(3 * j + m, 3 * j + n, H(m + 3, n + 3)));
            }
        }
    }
    K.setFromTriplets(tripletList.begin(), tripletList.end());

}