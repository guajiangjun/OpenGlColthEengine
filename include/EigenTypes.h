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
    using Point3 = Eigen::Vector3f;

}


Eigen::Point3 eigenVector(const glm::vec3& vec) {
    return Eigen::Point3(vec.x, vec.y, vec.z);
}
