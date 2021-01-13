#include "stdafx.h"
#include "Geom.h"
#include <iostream>
#include <queue>
#include "Utility.h"
#include <osg/Geometry>
#include "Triangle.h"
#include <queue>
#include <set>
#include <osg/CoordinateSystemNode>
#include <fstream>
#include <string>
#include <osgViewer\View>
#include <sstream>
#include <limits>
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

double distance(vector<double>& a, vector<double> b);

double cross_product(vector<double>& p1, vector<double>& p2, vector<double>& p3);

bool comp1(const vector<double>& a, vector<double>& b);

bool comp2(const vector<double>& a, vector<double>& b);

void Graham_scan(vector<vector<double>>& points, vector<vector<double>>& convex, vector<vector<double>>& non_convex);

osg::Vec3d WorldToLonLatAlt(const osg::Vec3d world);

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

	int min_height = findModelMinHeight();
	int max_height = findModelMaxHeight();

	
	int count = 0;
	
	for (int i = 0; i < vertices.size(); i++)
	{
		//if ((vertices[i]->coor.z()< max_height)&& (vertices[i]->coor.z() > min_height))
		//{
				vertexArray->push_back(vertices[i]->coor);
				normalArray->push_back(vertices[i]->normal);
				cout << vertices[i]->coor.x() <<"," <<vertices[i]->coor.y()<<","<< vertices[i]->coor.z()<<endl;
				count++;
		//}

	}

	//颜色
	colorArray->push_back(color);
	geometry->setVertexArray(vertexArray);
	geometry->setNormalArray(normalArray, osg::Array::BIND_PER_VERTEX);
	geometry->setColorArray(colorArray, osg::Array::BIND_OVERALL);
	geometry->setColorBinding(osg::Geometry::BIND_OVERALL);

	//将geom显示成点
	osg::ref_ptr<osg::PrimitiveSet> primitiveSet = new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, count);
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
	
	
	vector<vector<double>> coor;
	vector<vector<double>> convex;
	vector<vector<double>> non_convex;

	int min_height = findModelMinHeight();
	
	int max_height = findModelMaxHeight();

	min_height += (max_height - min_height) / 2;
	
	//每一个三角形都是一个geometry 针对于每一个三角形画出三个边
	for (int i=0;i<triangles.size();i++)
	{
			if ((vertices[triangles[i]->vertexIndexs[0]]->coor.z() > min_height) && (vertices[triangles[i]->vertexIndexs[1]]->coor.z() > min_height) &&
				(vertices[triangles[i]->vertexIndexs[2]]->coor.z() > min_height)&& (vertices[triangles[i]->vertexIndexs[0]]->coor.z() < max_height)&&
				(vertices[triangles[i]->vertexIndexs[1]]->coor.z() < max_height)&& (vertices[triangles[i]->vertexIndexs[2]]->coor.z() < max_height))
			{
				
				buildings.building_triangles.push_back(triangles[i]);
				buildings.belong.push_back(0);
			}
	}	

	if (buildings.building_triangles.size()>0)
	{
		cout << "building_triangles.size:" << buildings.building_triangles.size() << endl;

		
		osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
		osg::ref_ptr<osg::Vec3Array> vertexArray = new osg::Vec3Array;
		osg::ref_ptr<osg::Vec3Array> normalArray = new osg::Vec3Array;
		osg::ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array;
		osg::ref_ptr<osg::Vec2Array> textCoorArray = new osg::Vec2Array;
		
		
		
		

		list<Triangle*>::iterator itor_triangles = buildings.building_triangles.begin();
		list<int>::iterator itor_belong = buildings.belong.begin();
		triangle_queue.push(*itor_triangles);

		while (!isDone(buildings))
		{

			if (triangle_queue.size() == 0) {
				groups++;

				while (itor_triangles != buildings.building_triangles.end()) {
					if (*itor_belong == 0) {
						triangle_queue.push(*itor_triangles);
						break;
					}
					itor_triangles++;
					itor_belong++;
				}

			}
			findAllAdjacent(triangle_queue.front());
			triangle_queue.pop();
		}

		itor_belong = buildings.belong.begin();
		while (itor_belong != buildings.belong.end())
		{
			cout << *itor_belong;
			itor_belong++;
		}
		cout << "geom===============" << endl;


		itor_belong = buildings.belong.begin();
		itor_triangles = buildings.building_triangles.begin();
		while (itor_triangles != buildings.building_triangles.end())
		{
			

			if (*itor_belong == 1) {
				
				coor.push_back({ vertices[(*itor_triangles)->vertexIndexs[0]]->coor.x(), vertices[(*itor_triangles)->vertexIndexs[0]]->coor.y() });
				coor.push_back({ vertices[(*itor_triangles)->vertexIndexs[1]]->coor.x(), vertices[(*itor_triangles)->vertexIndexs[1]]->coor.y() });
				coor.push_back({ vertices[(*itor_triangles)->vertexIndexs[2]]->coor.x(), vertices[(*itor_triangles)->vertexIndexs[2]]->coor.y() });

				//cout << vertices[(*itor_triangles)->vertexIndexs[0]]->coor.x() << "," << vertices[(*itor_triangles)->vertexIndexs[0]]->coor.y() << endl;
				//cout << vertices[(*itor_triangles)->vertexIndexs[1]]->coor.x() << "," << vertices[(*itor_triangles)->vertexIndexs[1]]->coor.y() << endl;
				//cout << vertices[(*itor_triangles)->vertexIndexs[2]]->coor.x() << "," << vertices[(*itor_triangles)->vertexIndexs[2]]->coor.y() << endl;

				

				
				vertices[(*itor_triangles)->vertexIndexs[0]]->normal = (*itor_triangles)->normal;
				vertices[(*itor_triangles)->vertexIndexs[1]]->normal = (*itor_triangles)->normal;
				vertices[(*itor_triangles)->vertexIndexs[2]]->normal = (*itor_triangles)->normal;



				vertexArray->push_back(vertices[(*itor_triangles)->vertexIndexs[0]]->coor);
				vertexArray->push_back(vertices[(*itor_triangles)->vertexIndexs[1]]->coor);
				vertexArray->push_back(vertices[(*itor_triangles)->vertexIndexs[2]]->coor);
				//normalArray->push_back(vertices[(*itor_triangles)->vertexIndexs[0]]->normal);
				//normalArray->push_back(vertices[(*itor_triangles)->vertexIndexs[1]]->normal);
				//normalArray->push_back(vertices[(*itor_triangles)->vertexIndexs[2]]->normal);
				
				//顶点的法向量数据全是0，由于一个三角形是2d的图形，所以我直接让一个三角形的三个顶点的法向量数据等于三角形的法向量
			
			}
			

			itor_triangles++;
			itor_belong++;
		}

		set<osg::Vec3>s(vertexArray->begin(), vertexArray->end());
		vertexArray->assign(s.begin(), s.end());

		osg::ref_ptr<osg::PrimitiveSet> primitiveSet = new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, s.size());

		
		
		colorArray->push_back(color);
		geometry->setVertexArray(vertexArray);
		//geometry->setNormalArray(normalArray, osg::Array::BIND_PER_VERTEX);
		geometry->setColorArray(colorArray, osg::Array::BIND_OVERALL);
		geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
		osg::StateSet* stateSet = geometry->getOrCreateStateSet();
		osg::Point* pointSize = new osg::Point;
		pointSize->setSize(4.0);
		stateSet->setAttribute(pointSize);
		geometry->addPrimitiveSet(primitiveSet);
		geode->addDrawable(geometry);
		
		

		//删掉重复的点（因为是直接投影的可能有z值不同但x，y值相同的点）
		if (coor.size() >= 3) {
			cout << "coor size before:" << coor.size() << endl;
			for (int i = 0; i < coor.size() - 1; i++)
			{
				for (int j = i + 1; j < coor.size(); j++)
				{
					if ((coor[i][0] == coor[j][0]) && (coor[i][1] == coor[j][1]))
					{
						auto it = remove(coor.begin(), coor.end(), coor[j]);
						coor.erase(it, coor.end());
						
						cout << "sameeeeeeeeeeeeeeeeeeeeeeeeeeeee" << endl;
					}

				}
			}
		}
		

		









		cout << "here" << endl;
		cout << "coor.size after:" << coor.size() << endl;
		if (coor.size() >= 3) {
			Graham_scan(coor, convex, non_convex);
		}
		else {
			convex = coor;
		}

		//删掉重复的点（因为是直接投影的可能有z值不同但x，y值相同的点）
		if (convex.size() >= 3) {
			cout << "convex size before:" << convex.size() << endl;
			for (int i = 0; i <convex.size() - 1; i++)
			{
				for (int j = i + 1; j < convex.size(); j++)
				{
					if ((convex[i][0] == convex[j][0]) && (convex[i][1] == convex[j][1]))
					{
						auto it = remove(convex.begin(), convex.end(), convex[j]);
						convex.erase(it, convex.end());
					}

				}
			}


			

		}

		cout << "out:::::::::" << endl;
		for (int i = 0; i < convex.size(); i++)
		{
			cout << convex[i][0] << ",";
			cout << convex[i][1] << endl;
		}

		if (convex.size()>=3)
		{
			osg::ref_ptr<osg::Geometry> geometry_contour = new osg::Geometry;
			osg::ref_ptr<osg::Vec3Array> vertexArray_contour = new osg::Vec3Array;
			osg::ref_ptr<osg::Vec3Array> normalArray_contour = new osg::Vec3Array;
			osg::ref_ptr<osg::Vec4Array> colorArray_contour = new osg::Vec4Array;
			osg::ref_ptr<osg::Vec2Array> textCoorArray_contour = new osg::Vec2Array;
			ofstream outfile("C:\\Users\\dwyane\\Desktop\\out.mif", ios::app);
			ofstream outfile1("C:\\Users\\dwyane\\Desktop\\out.mid", ios::app);
			ifstream infile("C:\\Users\\dwyane\\Desktop\\out.mid");

			int n = 0;//用来表示说读/写文本的行数
			string tmp;
			if (infile.eof()) {
				outfile1 << 1 << endl;
			}
			while (getline(infile, tmp, '\n'))
			{
				cout << "......" << endl;
				n++;
			}
			infile.close();
			cout << "行数:" << n;
			int k = n + 1;
			outfile1 << k << endl;
			outfile1.close();

			outfile << "Region 1" << endl;
			outfile << "  " << convex.size() << endl;
			osg::Vec3f c;
			for (int i = 0; i < convex.size(); i++) {


				c.x() = convex[i][0];
				c.y() = convex[i][1];
				c.z() = findModelMinHeight();
				vertexArray_contour->push_back(c);
				ostringstream out;
				int prec = numeric_limits<double>::digits10;
				out.precision(prec);
				out << convex[i][0] + 411229 << " " << convex[i][1] + 2382118;
				string str = out.str();
				outfile << str << endl;

			}
			if (convex.size() >= 1) {
				c.x() = convex[0][0];
				c.y() = convex[0][1];
				c.z() = findModelMinHeight();
				vertexArray_contour->push_back(c);
			}

			outfile << "    Pen (1,2,0)" << endl;
			outfile << "    Brush(1,0,16777215)" << endl;
			outfile.close();

			osg::ref_ptr<osg::PrimitiveSet> primitiveSet_contour = new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 0, vertexArray_contour->size());
			colorArray_contour->push_back(color);
			geometry_contour->setVertexArray(vertexArray_contour);
			geometry_contour->setColorBinding(osg::Geometry::BIND_OVERALL);
			geometry_contour->addPrimitiveSet(primitiveSet_contour);
			//geometry->setTexCoordArray(0, textCoorArray);
			geode->addDrawable(geometry_contour);
		}

		
		
	}
	

	return geode;
}








bool Geom::isTwoTriangleNeighbor(Triangle* triangle1,Triangle* triangle2)
{

	osg::Vec3 pnt11 = vertices.at(triangle1->vertexIndexs[0])->coor;
	osg::Vec3 pnt12 = vertices.at(triangle1->vertexIndexs[1])->coor;
	osg::Vec3 pnt13 = vertices.at(triangle1->vertexIndexs[2])->coor;
	
	osg::Vec3 pnt21 = vertices.at(triangle2->vertexIndexs[0])->coor;
	osg::Vec3 pnt22 = vertices.at(triangle2->vertexIndexs[1])->coor;
	osg::Vec3 pnt23 = vertices.at(triangle2->vertexIndexs[2])->coor;


	


	if ((Utility::isVec3Same(pnt11, pnt21) && Utility::isVec3Same(pnt12, pnt22))//第一条边
		|| (Utility::isVec3Same(pnt11, pnt22) && Utility::isVec3Same(pnt12, pnt21))
		|| (Utility::isVec3Same(pnt11, pnt22) && Utility::isVec3Same(pnt12, pnt23))//第二条边
		|| (Utility::isVec3Same(pnt11, pnt23) && Utility::isVec3Same(pnt12, pnt22))
		|| (Utility::isVec3Same(pnt11, pnt21) && Utility::isVec3Same(pnt12, pnt23))//第三条边
		|| (Utility::isVec3Same(pnt11, pnt23) && Utility::isVec3Same(pnt12, pnt21))

		|| (Utility::isVec3Same(pnt12, pnt21) && Utility::isVec3Same(pnt13, pnt22))//第一条边
		|| (Utility::isVec3Same(pnt12, pnt22) && Utility::isVec3Same(pnt13, pnt21))
		|| (Utility::isVec3Same(pnt12, pnt22) && Utility::isVec3Same(pnt13, pnt23))//第二条边
		|| (Utility::isVec3Same(pnt12, pnt23) && Utility::isVec3Same(pnt13, pnt22))
		|| (Utility::isVec3Same(pnt12, pnt21) && Utility::isVec3Same(pnt13, pnt23))//第三条边
		|| (Utility::isVec3Same(pnt12, pnt23) && Utility::isVec3Same(pnt13, pnt21))

		|| (Utility::isVec3Same(pnt11, pnt21) && Utility::isVec3Same(pnt13, pnt22))//第一条边
		|| (Utility::isVec3Same(pnt11, pnt22) && Utility::isVec3Same(pnt13, pnt21))
		|| (Utility::isVec3Same(pnt11, pnt22) && Utility::isVec3Same(pnt13, pnt23))//第二条边
		|| (Utility::isVec3Same(pnt11, pnt23) && Utility::isVec3Same(pnt13, pnt22))
		|| (Utility::isVec3Same(pnt11, pnt21) && Utility::isVec3Same(pnt13, pnt23))//第三条边
		|| (Utility::isVec3Same(pnt11, pnt23) && Utility::isVec3Same(pnt13, pnt21)))
		return true;
	return  false;
}

void Geom::findAllAdjacent(Triangle* triangle)
{
	int mcount = 0;
	list<Triangle*>::iterator itor_triangles = buildings.building_triangles.begin();
	list<int>::iterator itor_belong = buildings.belong.begin();
	while (itor_triangles!=buildings.building_triangles.end())
	{
		if (*itor_belong==0&&(*itor_triangles!=triangle))
		{
			if (isTwoTriangleNeighbor(*itor_triangles, triangle))
			{
				triangle_queue.push(*itor_triangles);
				*itor_belong = groups;
				mcount++;
				
			}
		}
		itor_belong++;
		itor_triangles++;
	}

	
	
		if ((mcount==0))
	{
		itor_belong = buildings.belong.begin();
		itor_triangles = buildings.building_triangles.begin();
		while (itor_triangles!=buildings.building_triangles.end())
		{
			if (*itor_triangles == triangle&&(*itor_belong==0)) {
				*itor_belong = 999;
				cout << "single Triangle" << endl;
			}
			itor_belong++;
			itor_triangles++;
		}
		
	}
	
	
}


bool Geom::isDone(Buildings b) {

	list<int>::iterator itor = buildings.belong.begin();
	while (itor!=buildings.belong.end())
	{
		if (*itor==0)
		{
			return false;
		}
		itor++;
	}
	return true;
}




int Geom::findModelMinHeight()
{

	int minh = 100000;

	for (Vertex* vertex : vertices) {
		if (vertex->coor.z() < minh) {
				minh = vertex->coor.z();				
		}
	}
	return minh;
}

int Geom::findModelMaxHeight()
{
	int maxh = 0;
	for (Vertex* vertex : vertices) {
		if (vertex->coor.z() > maxh) {
			maxh = vertex->coor.z();
		}
	}

	
	return maxh;
}


bool comp1(const vector<double>& a, vector<double>& b) {
	return a[0] < b[0] || (a[0] == b[0] && a[1] < b[1]);
}

bool comp2(const vector<double>& a, vector<double>& b) {
	return a[1] < b[1] || (a[1] == b[1] && a[0] < b[0]);
}

double distance(vector<double>& a, vector<double> b) {
	double tmp = pow(a[0] - b[0], 2) + pow(a[1] - b[1], 2);
	return sqrt(tmp);
}
double cross_product(vector<double>& p1, vector<double>& p2, vector<double>& p3) {
	double x1 = p2[0] - p1[0], x2 = p3[0] - p1[0], y1 = p2[1] - p1[1], y2 = p3[1] - p1[1];
	return x1 * y2 - x2 * y1;

}
void Graham_scan(vector<vector<double>>& points, vector<vector<double>>& convex, vector<vector<double>>& non_convex) {
	int point_num = points.size();
	sort(points.begin(), points.end(), comp2);
	vector<double> init = points[0];
	vector<vector<double>> attr(point_num - 1, vector<double>(3, 0));
	for (int i = 1; i < points.size(); i++) {
		double degree = atan((points[i][1] - init[1]) / (points[i][0] - init[0]));
		double pi = 4 * atan(1);
		degree = degree > 0 ? degree : degree + pi;
		double dis;
		dis = distance(points[i], init);
		attr[i - 1][0] = degree / pi * 180;
		attr[i - 1][1] = dis;
		attr[i - 1][2] = i;
	}
	sort(attr.begin(), attr.end(), comp1);
	//vector<vector<double>> non_convex;
	vector<vector<double>> convex_tmp;

	for (int i = 1; i < attr.size(); i++) {
		if (attr[i][0] == attr[i - 1][0]) {
			non_convex.push_back(points[attr[i - 1][2]]);
		}
	}

	convex_tmp.push_back(points[attr[attr.size() - 1][2]]);

	for (int i = attr.size() - 1; i >= 1; i--) {
		if (attr[i][0] != attr[i - 1][0])
			convex_tmp.push_back(points[attr[i - 1][2]]);
	}
	convex_tmp.push_back(init);
	reverse(convex_tmp.begin(), convex_tmp.end());

	/*
	//不把一条射线上距离近的点删去，可以求得正确的凸点集，但是复杂度会增加。
	convex_tmp.push_back(init);
	for(int i=0; i<attr.size(); i++){
		convex_tmp.push_back(points[attr[i][2]]);
	}
	*/
	//vector<vector<double>> convex;
	convex.push_back(convex_tmp[0]);
	convex.push_back(convex_tmp[1]);
	convex.push_back(convex_tmp[2]);
	int top = 2;
	for (int i = 3; i < convex_tmp.size(); i++) {
		while (cross_product(convex[top - 1], convex[top], convex_tmp[i]) < 0) {
			non_convex.push_back(*(convex.begin() + top));
			convex.erase(convex.begin() + top);
			top--;
		}
		convex.push_back(convex_tmp[i]);
		top++;
	}
	// 把最小和最大角所在射线上的所有点都归为凸集点
	double min_angle = attr[0][0], max_angle = attr[attr.size() - 1][0];
	double min_angle_max_distance = distance(convex[0], convex[1]);
	int count = 0;
	for (int i = attr.size() - 1; i >= 0; i--) {
		if (attr[i][0] == max_angle) {
			if (count == 0)
				count = 1;
			else
				convex.push_back(points[attr[i][2]]);
		}
	}
	for (int i = 0; i < attr.size(); i++) {
		if (attr[i][0] == min_angle && attr[i][1] != min_angle_max_distance) {
			convex.insert(convex.begin() + count, points[attr[i][2]]);
			count++;
		}
	}
	for (int i = 0; i < non_convex.size(); i++) {
		double pi = 4 * atan(1);
		double degree = atan((non_convex[i][1] - init[1]) / (non_convex[i][0] - init[0]));
		degree = degree > 0 ? degree / pi * 180 : (degree + pi) / pi * 180;
		if (degree == max_angle || degree == min_angle) {
			non_convex.erase(non_convex.begin() + i);
			i--;
		}
	}
}


// 世界坐标转经纬度
osg::Vec3d WorldToLonLatAlt(const osg::Vec3d world)
{
	osg::EllipsoidModel* em = new osg::EllipsoidModel();
	osg::Vec3d lonLatAlt;
	em->convertXYZToLatLongHeight(world.x(), world.y(), world.z(),
		lonLatAlt.y(), lonLatAlt.x(), lonLatAlt.z());

	lonLatAlt.x() = osg::RadiansToDegrees(lonLatAlt.x());
	lonLatAlt.y() = osg::RadiansToDegrees(lonLatAlt.y());
	return lonLatAlt;
}


