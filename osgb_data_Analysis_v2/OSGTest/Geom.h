/*****************************************************************************
* @brief: Geom是一个不可再分的建筑物，有顶点、三角形、包围盒信息
* @author: Zhengyang Wang
* @date: 2020/12/21 9:51
* @version: ver 1.0
* @inparam: 
* @outparam: 
*****************************************************************************/
#pragma once
#include <vector>
#include "Vertex.h"
#include "Triangle.h"
#include <osg/ref_ptr>
#include <osg/Geode>
#include <osg/LineWidth>
#include <osg/Point>
#include <queue>
#include "Buildings.h"
#include <iostream>
#include <numeric>
#include <algorithm>
#include <limits>
#include <cmath>
#include <stack>
#include <osgUtil/LineSegmentIntersector>
using namespace std;


class Geom
{
public:
	vector<Vertex*> vertices;
	vector<Triangle*> triangles;
	osg::BoundingBox  boundingBox;
	vector<vector<Triangle*>> buildingList;
	Buildings buildings;
	queue<Triangle*> triangle_queue;
	int groups=1;
	osg::ref_ptr<osg::Geode> createOsgNode(osg::Vec4 color);
	osg::ref_ptr<osg::Geode> Geom::createOsgNode_Point(osg::Vec4 color);
	osg::ref_ptr<osg::Geode> Geom::createOsgNode_Triangle(osg::Vec4 color);
	bool isTwoTriangleNeighbor(Triangle*, Triangle*);
	void findAllAdjacent(Triangle*);
	//osg::ref_ptr<osg::Geode> Geom::createOsgNode_building(osg::Vec4 color);
	//osg::ref_ptr<osg::Vec3Array> findTriangleCenter();
	
	bool isDone(Buildings b);
	int findModelMinHeight();
	int findModelMaxHeight();


	Geom();
	~Geom();
};

