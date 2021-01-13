/*****************************************************************************
* @brief: 访问器，用于获取模型的所有信息 顶点、法向量、纹理坐标
* @author: Zhengyang Wang
* @date: 2020/12/21 9:55
* @version: ver 1.0
* @inparam: 
* @outparam: 
*****************************************************************************/
#pragma once
#include <osg/Drawable>

class ModelAttributeFunctor
	:public osg::Drawable::AttributeFunctor
{
public:
	osg::ref_ptr<osg::Vec3Array> vertexList;
	osg::ref_ptr<osg::Vec3Array> normalList;
	osg::ref_ptr<osg::Vec2Array> textCoordList;
	virtual void apply(osg::Drawable::AttributeType, unsigned, osg::Vec2*) override;
	virtual void apply(osg::Drawable::AttributeType, unsigned, osg::Vec3*) override;
	ModelAttributeFunctor();
	~ModelAttributeFunctor();
};

