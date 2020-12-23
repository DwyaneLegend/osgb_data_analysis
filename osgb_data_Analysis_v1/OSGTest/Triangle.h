/*****************************************************************************
* @brief: 自定义三角形类，存储三角形顶点所在索引，法向量，三角形在数组中的索引
* @author: Zhengyang Wang
* @date: 2020/12/21 10:25
* @version: ver 1.0
* @inparam: 
* @outparam: 
*****************************************************************************/
#pragma once
#include <osg/Vec3>
#include <vector>
#include <osg/Vec4>

class Triangle
{
public:
	void init();
	Triangle();
	~Triangle();
	int vertexIndexs[3];
	osg::Vec3 normal;
	int index;//该三角形在数组中的索引
	std::vector<int> neighborTriangles;//相邻的三角形的索引
};

