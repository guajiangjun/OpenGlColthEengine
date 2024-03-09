#pragma once

#include"EigenTypes.h"
#include"Drawer.h"
class Ray {
public:
    Ray() {}

    Ray(const Eigen::Vector3f& origin, const Eigen::Vector3f& direction) : orig(origin), dir(direction) {
        dir = dir.normalized();
    }

    Eigen::Vector3f at(double t) const {
        return orig + t * dir;
    }

    float compute_distance(const Eigen::Vector3f& point)const  {
        Eigen::Vector3f v = point - orig;
        float t = v.dot(dir);

        Eigen::Vector3f closest_point_on_ray = orig + t * dir;
        return (point - closest_point_on_ray).norm();
    }
    void draw() {
        drawer.DrawLine(orig, this->at(10));
    }
public:
    Eigen::Vector3f orig;
    Eigen::Vector3f dir;
    Drawer drawer;
};

