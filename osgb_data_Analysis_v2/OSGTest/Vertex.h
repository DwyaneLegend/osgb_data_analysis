/*****************************************************************************
* @brief: 顶点类，拥有坐标，法向量，纹理坐标，顶点在数组中的下标，相邻三角形这些属性
* @author: Zhengyang Wang
* @date: 2020/12/21 10:29
* @version: ver 1.0
* @inparam: 
* @outparam: 
*****************************************************************************/
#pragma once

#include <osg/Vec3>
#include <osg/Vec2>
#include <osg/Vec4>
#include <vector>
using namespace std;


class Vertex
{
public:
	osg::Vec3 coor;
	osg::Vec3 normal;
	osg::Vec2 texCoor;
	int index;//该顶点在数组中的下标
	vector<int> neighborTriangle;//顶点相邻的三角形
	Vertex();
	~Vertex();
};

