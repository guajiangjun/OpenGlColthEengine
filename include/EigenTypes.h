#pragma once


#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/Core>
namespace Eigen {

    //dense types
    
    using Vector6f = Eigen::Matrix<float, 6, 1>;
    using Matrix36f = Eigen::Matrix<float, 3, 6>;
    
    using Matrix66f = Eigen::Matrix<float, 6, 6>;
    using Matrix44f = Eigen::Matrix<float, 4, 4>;
    //sparse types
   
    using SparseMatrixf = Eigen::SparseMatrix<float>;

}

template<typename T>
void printMatRow(const T& mat,int n) {
    for (int i = 0;i < n;i++) {
        cout << i << ":\t" << mat.row(i) << endl;
    }
}


template<typename T>
void printMatCol(const T& mat, int n) {
    for (int i = 0;i < n;i++) {
        cout << i << ":\t" << mat.col(i).transpose() << endl;
    }
}
void printQ(const Eigen::VectorXf& q) {
    for (int i = 0;i < q.size()/3;i++) {
        cout << i << ":\t";
        cout << q.segment<3>(3 * i).transpose() << endl;
    }
}