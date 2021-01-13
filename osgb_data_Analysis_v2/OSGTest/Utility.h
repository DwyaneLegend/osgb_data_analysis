/*****************************************************************************
* @brief: 工具类，从模型路径中获得名称、判断三维向量是否相等
* @author: Zhengyang Wang
* @date: 2020/12/21 10:27
* @version: ver 1.0
* @inparam: 
* @outparam: 
*****************************************************************************/
#pragma once
#include <osg/Vec3>
#include <string>
#include <osgText/Text>
using namespace std;

class Utility
{
public:
	static bool isVec3Same(osg::Vec3 v1, osg::Vec3 v2);//比较两个三维向量是否相等
	static string getFileNameFromPath(string path);//从模型路径中获取明名称
	static void string_replace(std::string &strBig, const std::string &strsrc, const std::string &strdst);
	static osg::ref_ptr<osgText::Text> createText(string info, int textSize, osg::ref_ptr<osgText::Font>font, osg::Vec3 position, int direction, osg::Vec4 textColor);
	static osg::ref_ptr<osg::Node> createCoorAxis(int textSize);
	Utility();
	~Utility();
};

