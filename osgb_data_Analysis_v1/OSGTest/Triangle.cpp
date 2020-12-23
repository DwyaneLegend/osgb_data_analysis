#include "stdafx.h"
#include "Triangle.h"

/*****************************************************************************
* @brief: 三角形类
* @author: Zhengyang Wang
* @date: 2020/12/21 9:43
* @version: ver 1.0
* @inparam: 
* @outparam: 
*****************************************************************************/
Triangle::Triangle()
{
	init();
}


Triangle::~Triangle()
{
}

void Triangle::init()
{
	this->vertexIndexs[0] = this->vertexIndexs[1] = this->vertexIndexs[2] = -1;
	this->normal.set(0.0f, 0.0f, 0.0f);
}

