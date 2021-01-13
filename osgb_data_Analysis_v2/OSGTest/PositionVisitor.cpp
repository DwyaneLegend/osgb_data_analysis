#include "stdafx.h"
#include "PositionVisitor.h"
#include <osg/Drawable>
#include <osg/Geode>
#include <iostream>
#include "Geom.h"
#include <valarray>
#include "Triangle.h"
#include "Vertex.h"
#include <cstdlib>
#include <ctime>
#include <osg/ComputeBoundsVisitor>
#include "Utility.h"

PositionVisitor::PositionVisitor(string ModelName)
{
	this->modelName = ModelName;
	setTraversalMode(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN);
}

PositionVisitor::PositionVisitor(string ModelName, int TextSize)
{
	this->modelName = ModelName;
	this->textSize = TextSize;
	setTraversalMode(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN);
}


PositionVisitor::~PositionVisitor()
{
	for (Geom* geom:allGeom)
	{
		delete geom;
	}
}

void PositionVisitor::apply(osg::Geode& node)
{
	for (size_t i=0;i<node.getNumDrawables();i++)
	{
		osg::ref_ptr<osg::Drawable> drawable = node.getDrawable(i);
		ModelAttributeFunctor functor;
		drawable->accept(functor);
		osg::TriangleIndexFunctor<TriangleIndex> triangleIndex;
		drawable->accept(triangleIndex);
		dealTriangleInfo(functor, triangleIndex);
	}
}

void PositionVisitor::dealTriangleInfo(ModelAttributeFunctor attributeFunctor, osg::TriangleIndexFunctor<TriangleIndex> indexFunctor)
{
	Geom *geom = new Geom;
	//cout<<attributeFunctor.textCoordList->size() << endl;
	if (attributeFunctor.textCoordList->size()!=0
		&&attributeFunctor.textCoordList->size()!=attributeFunctor.vertexList->size())
	{
		cout << "纹理坐标和顶点数量不匹配" << endl;
		return;
	}
	//处理顶点信息
	for (size_t i=0;i<attributeFunctor.vertexList->size();i++)
	{
		Vertex* vertex=new Vertex;
		vertex->coor = attributeFunctor.vertexList->at(i);
		vertex->index = i;
		if (attributeFunctor.normalList!=NULL&&attributeFunctor.normalList->size()>i)
			vertex->normal = attributeFunctor.normalList->at(i);
		if (i< attributeFunctor.textCoordList->size())
			vertex->texCoor = attributeFunctor.textCoordList->at(i);
		geom->vertices.push_back(vertex);
	}
	//处理三角形信息
	for (int i=0;i<indexFunctor.triangleNum;i++)
	{
		Triangle* triangle=new Triangle;
		triangle->index = i;
		triangle->vertexIndexs[0] = indexFunctor.indexs->at(i * 3);
		triangle->vertexIndexs[1] = indexFunctor.indexs->at(i * 3+1);
		triangle->vertexIndexs[2] = indexFunctor.indexs->at(i * 3+2);
		//计算法向量
		osg::Vec3 edge1 = geom->vertices.at(triangle->vertexIndexs[1])->coor - geom->vertices.at(triangle->vertexIndexs[0])->coor;
		osg::Vec3 edge2 = geom->vertices.at(triangle->vertexIndexs[2])->coor - geom->vertices.at(triangle->vertexIndexs[0])->coor;
		osg::Vec3 triangleNormal = edge1^edge2;
		triangleNormal.normalize();
		triangle->normal = triangleNormal;
		geom->triangles.push_back(triangle);
	}
	allGeom.push_back(geom);
}

osg::ref_ptr<osg::Node> PositionVisitor::createOsgNode(osg::Vec4 color, int order)
{
	this->geomColor = color;
	short direction = order % 4;
	osg::ref_ptr<osg::Group> result = new osg::Group;
	if (allGeom.size()>0&&allGeom.size()==1)
	{
		osg::ref_ptr<osg::Geode> geode= allGeom[0]->createOsgNode_Triangle(color);
		this->boundingBox = geode->getBoundingBox();
		result->addChild(geode);
		cout <<"bbx:"<< this->boundingBox.center().x() << "," << this->boundingBox.center().y() << "," << this->boundingBox.center().z() << endl;
	}
	else
	{
		
		for (Geom* geom : allGeom) {
			osg::Vec4 color1 = osg::Vec4(rand() % 10 * 0.1, rand() % 10 * 0.1, rand() % 10 * 0.1, 1.0f);
			osg::ref_ptr<osg::Geode> geode = geom->createOsgNode_Triangle(color1);
			result->addChild(geode);
			
		}			
		osg::ComputeBoundsVisitor boundsVisitor;
		result->accept(boundsVisitor);
		this->boundingBox = boundsVisitor.getBoundingBox();
		cout << "bbx:"<<this->boundingBox.center().x() << "," << this->boundingBox.center().y() << "," << this->boundingBox.center().z() << endl;
	}
	//result->addChild(createTipText(direction));
	return result;
}

osg::ref_ptr<osg::Node> PositionVisitor::createRandomColorOsgNode(int order)
{
	//创建一个随机颜色
	osg::Vec4 color = osg::Vec4(rand()%10*0.1, rand() % 10 * 0.1, rand() % 10 * 0.1, 1.0f);
		this->geomColor = color;
		return createOsgNode(color, order);	
}

osg::ref_ptr<osgText::Text> PositionVisitor::createTipText(short direction)
{
	osg::ref_ptr<osgText::Font> font = osgText::readFontFile("fonts/simhei.ttf");
	//对每个组件设置不同的朝向，避免所有的提示文字都在一个朝向
	osg::Vec3 tipPosition;
	//osg::BoundingBox bbx = this->boundingBox;
	/*
	float halfX = -806.602;
	float halfY = -231.751;
	float halfZ = 29.6532;
	*/
	float halfX = (boundingBox.xMax() + boundingBox.xMin()) / 2;
	float halfY = (boundingBox.yMax() + boundingBox.yMin()) / 2;
	float halfZ = (boundingBox.zMax() + boundingBox.zMin()) / 2;

	
	
	
	
	switch (direction)
	{
	case 0://左
	
		tipPosition = osg::Vec3(halfX, boundingBox.yMin() - 1, halfZ);
		return Utility::createText(modelName, textSize, font, tipPosition, direction, this->geomColor);
	case 1://右
		
		tipPosition = osg::Vec3(halfX, boundingBox.yMax() + 1, halfZ);
		return Utility::createText(modelName, textSize, font, tipPosition, direction, this->geomColor);
	case 2://前
		
		tipPosition = osg::Vec3(boundingBox.xMax() + 1, halfY, halfZ);
		return Utility::createText(modelName, textSize, font, tipPosition, direction, this->geomColor);
	case 3://后
		
		tipPosition = osg::Vec3(boundingBox.xMin() - 1, halfY, halfZ);
		return Utility::createText(modelName, textSize, font, tipPosition, direction, this->geomColor);
	}
	return NULL;
}

osg::ref_ptr<osgText::Text> PositionVisitor::createCenterText(osg::BoundingBox bbx)
{
	osg::ref_ptr<osgText::Font> font = osgText::readFontFile("fonts/simhei.ttf");
	//对每个组件设置不同的朝向，避免所有的提示文字都在一个朝向
	osg::Vec3 tipPosition;
	float halfX = (bbx.xMax() + bbx.xMin()) / 2;
	float halfY = (bbx.yMax() + bbx.yMin()) / 2;
	float halfZ = (bbx.zMax() + bbx.zMin()) / 2;
	tipPosition = osg::Vec3(boundingBox.xMax() + 1, halfY, halfZ);
	return Utility::createText("zhongxing", textSize, font, tipPosition, 2, this->geomColor);
	
	
}




