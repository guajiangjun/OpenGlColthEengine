#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include"shader_m.h"

#include <Eigen\Dense>


class Line
{
public:
	
	Eigen::Vector3f m_point0;
	Eigen::Vector3f m_point1;

	Line(
		const Eigen::Vector3f& point0,
		const Eigen::Vector3f& point1
	)
		:m_point0(point0),m_point1(point1)
	{
		
	}
	Line() {}

};



