#include "stdafx.h"
#include "Geom.h"
#include <iostream>
#include <queue>
#include "Utility.h"
#include <osg/Geometry>
#include "Triangle.h"
using namespace std;

Geom::Geom()
{
}

Geom::~Geom()
{
	for (Vertex* vertex : vertices)
		delete vertex;
	for (Triangle* triangle : triangles)
		delete triangle;
}

osg::ref_ptr<osg::Geode> Geom::createOsgNode(osg::Vec4 color)
{
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
	//顶点、法向量
	osg::ref_ptr<osg::Vec3Array> vertexArray = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec3Array> normalArray = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array;
	osg::ref_ptr<osg::Vec2Array> textCoorArray = new osg::Vec2Array;

	//索引
	osg::ref_ptr<osg::DrawElementsUInt> indexs = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
	for (Triangle* triangle : triangles)
	{
		indexs->push_back(triangle->vertexIndexs[0]);
		indexs->push_back(triangle->vertexIndexs[1]);
		indexs->push_back(triangle->vertexIndexs[2]);
		//顶点的法向量数据全是0，由于一个三角形是2d的图形，所以我直接让一个三角形的三个顶点的法向量数据等于三角形的法向量
		vertices[triangle->vertexIndexs[0]]->normal = triangle->normal;
		vertices[triangle->vertexIndexs[1]]->normal = triangle->normal;
		vertices[triangle->vertexIndexs[2]]->normal = triangle->normal;
	}

	for (Vertex* vertex : vertices)
	{
		vertexArray->push_back(vertex->coor);
		normalArray->push_back(vertex->normal);
		textCoorArray->push_back(vertex->texCoor);
	}
	//颜色
	colorArray->push_back(color);
	
	cout <<"triangles:"<< triangles.size() << endl;
	cout << "vertices:"<<vertices.size() << endl;
	geometry->setVertexArray(vertexArray);
	geometry->setNormalArray(normalArray, osg::Array::BIND_PER_VERTEX);
	geometry->setColorArray(colorArray, osg::Array::BIND_OVERALL);
	geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
	geometry->addPrimitiveSet(indexs);
	geometry->setTexCoordArray(0,textCoorArray);
	geode->addDrawable(geometry);
	return geode;
}


osg::ref_ptr<osg::Geode> Geom::createOsgNode_Point(osg::Vec4 color) {
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	
	osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
	osg::ref_ptr<osg::Vec3Array> vertexArray = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec3Array> normalArray = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array;
	

	//顶点的法向量数据全是0，由于一个三角形是2d的图形，所以我直接让一个三角形的三个顶点的法向量数据等于三角形的法向量
	for (Triangle* triangle : triangles)
	{
		vertices[triangle->vertexIndexs[0]]->normal = triangle->normal;
		vertices[triangle->vertexIndexs[1]]->normal = triangle->normal;
		vertices[triangle->vertexIndexs[2]]->normal = triangle->normal;
	}

	for (Vertex* vertex : vertices)
	{
		vertexArray->push_back(vertex->coor);
		normalArray->push_back(vertex->normal);
		
	}
	//颜色
	colorArray->push_back(color);

	cout << "triangles:" << triangles.size() << endl;
	cout << "vertices:" << vertices.size() << endl;
	geometry->setVertexArray(vertexArray);
	geometry->setNormalArray(normalArray, osg::Array::BIND_PER_VERTEX);
	geometry->setColorArray(colorArray, osg::Array::BIND_OVERALL);
	geometry->setColorBinding(osg::Geometry::BIND_OVERALL);

	//将geom显示成点
	osg::ref_ptr<osg::PrimitiveSet> primitiveSet = new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, vertices.size());
	geometry->addPrimitiveSet(primitiveSet);
	osg::StateSet* stateSet = geometry->getOrCreateStateSet();
	osg::Point* pointSize = new osg::Point;
	pointSize->setSize(4.0);
	stateSet->setAttribute(pointSize);
	geometry->addPrimitiveSet(primitiveSet);
	geode->addDrawable(geometry);
	return geode;
}


osg::ref_ptr<osg::Geode> Geom::createOsgNode_Triangle(osg::Vec4 color)
{

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	list<osg::ref_ptr<osg::Geometry>> geometry_list;
	list<osg::ref_ptr<osg::Vec3Array>> vertexArray_list;
	list<osg::ref_ptr<osg::Vec3Array>> normalArray_list;
	list<osg::ref_ptr<osg::Vec4Array>> colorArray_list;



	//每一个三角形都是一个geometry 针对于每一个三角形画出三个边
	for (int i=0;i<triangles.size();i++)
	{
		geometry_list.push_back(new osg::Geometry);
		vertexArray_list.push_back(new osg::Vec3Array);
		normalArray_list.push_back(new osg::Vec3Array);
		colorArray_list.push_back(new osg::Vec4Array);

		//顶点的法向量数据全是0，由于一个三角形是2d的图形，所以我直接让一个三角形的三个顶点的法向量数据等于三角形的法向量
		vertices[triangles[i]->vertexIndexs[0]]->normal = triangles[i]->normal;
		vertices[triangles[i]->vertexIndexs[1]]->normal = triangles[i]->normal;
		vertices[triangles[i]->vertexIndexs[2]]->normal = triangles[i]->normal;

		vertexArray_list.back()->push_back(vertices[triangles[i]->vertexIndexs[0]]->coor);
		vertexArray_list.back()->push_back(vertices[triangles[i]->vertexIndexs[1]]->coor);
		vertexArray_list.back()->push_back(vertices[triangles[i]->vertexIndexs[2]]->coor);
		normalArray_list.back()->push_back(vertices[triangles[i]->vertexIndexs[0]]->normal);
		normalArray_list.back()->push_back(vertices[triangles[i]->vertexIndexs[1]]->normal);
		normalArray_list.back()->push_back(vertices[triangles[i]->vertexIndexs[2]]->normal);

		colorArray_list.back()->push_back(color);
		geometry_list.back()->setVertexArray(vertexArray_list.back());
		geometry_list.back()->setNormalArray(normalArray_list.back(), osg::Array::BIND_PER_VERTEX);
		geometry_list.back()->setColorArray(colorArray_list.back(), osg::Array::BIND_OVERALL);
		geometry_list.back()->setColorBinding(osg::Geometry::BIND_OVERALL);
		osg::ref_ptr<osg::PrimitiveSet> primitiveSet = new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 0, 3);
		geometry_list.back()->addPrimitiveSet(primitiveSet);
		geode->addDrawable(geometry_list.back());


	}
	return geode;
}









