/*****************************************************************************
* @brief: 访问器，遍历geom中三角形索引
* @author: Zhengyang Wang
* @date: 2020/12/21 10:23
* @version: ver 1.0
* @inparam: 
* @outparam: 
*****************************************************************************/
#pragma once
#include <osg/ref_ptr>
#include <osg/Array>
#include <osg/TriangleIndexFunctor>

class TriangleIndex
{
public:
	osg::ref_ptr<osg::UIntArray> indexs;
	int triangleNum;
	TriangleIndex();
	~TriangleIndex();
	void operator()(const unsigned int& v1, const unsigned int& v2, const unsigned int& v3);
};

