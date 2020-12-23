/*****************************************************************************
* @brief: 访问器，用于访问geom节点
* @author: Zhengyang Wang
* @date: 2020/12/21 10:03
* @version: ver 1.0
* @inparam: 
* @outparam: 
*****************************************************************************/
#pragma once
#include <osg/NodeVisitor>
#include <vector>
#include "ModelAttributeFunctor.h"
#include <osg/TriangleIndexFunctor>
#include "TriangleIndex.h"
#include "Geom.h"
#include <osgText/Text>
#include<osg/LineWidth>
using namespace std;
class PositionVisitor
	:public osg::NodeVisitor
{
protected:
	vector<Geom*> allGeom;
	osg::Vec4 geomColor;
	string modelName;
	int textSize;//提示文字的大小
	osg::BoundingBox boundingBox;
public:
	virtual  void apply(osg::Geode& node) override;
	void dealTriangleInfo(ModelAttributeFunctor attributeFunctor,osg::TriangleIndexFunctor<TriangleIndex> indexFunctor);//处理访问器得到的信息
	osg::ref_ptr<osg::Node> createOsgNode(osg::Vec4 color,int order);//根据指定的颜色，将geom中的数据创建成Geode
	osg::ref_ptr<osg::Node> createRandomColorOsgNode(int order);//将geom中的数据创建成osg节点，颜色随机
	osg::ref_ptr<osgText::Text> createTipText(short direction);//创建提示文字
	osg::ref_ptr<osgText::Text> createCenterText(osg::BoundingBox);//创建geom的中心点
	PositionVisitor(string ModelName);
	PositionVisitor(string ModelName, int TextSize);
	~PositionVisitor();
};

