#include "stdafx.h"
#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osgGA/StateSetManipulator>
#include <osgViewer/ViewerEventHandlers>
#include <vector>
#include <regex>
#include <windows.h>
#include "PositionVisitor.h"
#include "Utility.h"
#include "osgDB/WriteFile"
#include<io.h>
#include <string>
#include <iostream>

using namespace std;

const char* split = " ";

bool fileExists(const std::string fileName)
{
	ifstream fin(fileName);
	if (!fin)
		return false;
	return true;
}

/**
 * 从控制台读取模型的路径：所有模型的路径全是绝对路径，中间用逗号分隔
 */
vector<string> readAbsolutPath()
{
	vector<string> results;//存储所有要加载的模型的绝对路径
	cout << "请输入osgb根目录"<<endl;
	string modelPaths;
	getline(std::cin,modelPaths);
	std::regex re{ split };
	// 调用 std::vector::vector (InputIterator first, InputIterator last,const allocator_type& alloc = allocator_type())
	// 构造函数,完成字符串分割
	vector<string> splitResult = std::vector<std::string>{
		std::sregex_token_iterator(modelPaths.begin(), modelPaths.end(), re, -1),std::sregex_token_iterator()
	};
	for (string modelName : splitResult)
	{
		cout << modelName << endl;
		if (fileExists(modelName))
			results.push_back(modelName);
	}
	return results;
}

vector<string> findfile(string path, string mode)
{
	_finddata_t file;
	intptr_t HANDLE;
	string Onepath = path + mode;
	vector<string> results;
	vector<string> results1;
	HANDLE = _findfirst(Onepath.c_str(), &file);
	if (HANDLE == -1L)
	{
		cout << "can not match the folder path" << endl;
		system("pause");
	}
	do {
		//判断是否有子目录  
		if (file.attrib & _A_SUBDIR)
		{
			//判断是否为"."当前目录，".."上一层目录
			if ((strcmp(file.name, ".") != 0) && (strcmp(file.name, "..") != 0))
			{
				string newPath = path + "\\" + file.name;
				findfile(newPath, mode);
			}
		}
		else
		{
			results.push_back(path+"\\"+file.name);
			cout << path+"\\"+file.name << endl;

		}
	} while (_findnext(HANDLE, &file) == 0);
	_findclose(HANDLE);
	//results1.push_back("H:\\osgb\\Data\\Tile_+002_+003\\Tile_+002_+003_L19_00320t3.osgb");
	//results1.push_back("H:\\osgb\\Data\\Tile_+000_+003\\Tile_+000_+003_L20_000320t1.osgb");
	//results1.push_back("H:\\osgb\\Data\\Tile_+000_+003\\Tile_+000_+003_L20_001020t1.osgb");
	//results1.push_back("H:\\osgb\\Data\\Tile_+002_+003\\Tile_+002_+003_L20_003230.osgb");

	//results1.push_back("H:\\测试模型\\T064_OSGB\\T064_OSGB\\Data\\Tile_+002_+004\\Tile_+002_+004_L20_00100t3.osgb");
	results1.push_back("C:\\Users\\dwyane\\Desktop\\Tile_+002_+003_L19_00320t3.osgb");
	return results;
}

int main()
{
	srand((unsigned)time(NULL));
	int textSize = 2;
	ofstream outfile("C:\\Users\\dwyane\\Desktop\\out.mif", ios::app);
	outfile << "Version 300" << endl;
	outfile << "Charset \"Neutral\"" << endl;
	outfile << "Delimiter \",\"" << endl;
	outfile << "CoordSys Earth Projection 1, 104" << endl;
	outfile << "Columns 1" << endl;
	outfile << "  id Decimal(10,0)" << endl;
	outfile << "Data" << endl;
	outfile << endl;
	while (true)
	{
		vector<string> fileNames = findfile("H:\\data","\\*.*");
		//vector<string> fileNames = findfile("H:\\测试模型\\T064_OSGB\\T064_OSGB\\Data", "\\*.*");
		if (fileNames.size() == 0)
			cout << "输入的有效的文件路径为0" << endl;
		else
		{
			osgViewer::Viewer viewer;
			osg::ref_ptr<osg::Group> root = new osg::Group;
			osg::ref_ptr<osgDB::Options> options = new osgDB::Options("noRotation");
			osg::ref_ptr<osg::Node> node;
			string modelName;
			cout << fileNames.size() << endl;

			


			for (size_t i = 0; i<fileNames.size(); i++)
			{
				cout << "osgtext:filename.size:" << i << endl;
				string name = fileNames[i];
				node = osgDB::readNodeFile(name);
				
				modelName = Utility::getFileNameFromPath(name);


				PositionVisitor visitor = PositionVisitor(modelName,textSize);
				node->accept(visitor);
				cout << "osgtest---before" << endl;
				osg::ref_ptr<osg::Node> result = visitor.createRandomColorOsgNode(i);
				if (result!=NULL)
				{
					root->addChild(result);
				}
				
			    
				
				//root->addChild(node);
				
			}
			root->addChild(Utility::createCoorAxis(textSize));
			cout<<"num children"<<root->getNumChildren()<<endl;
			viewer.setSceneData(root);
			viewer.addEventHandler(new osgGA::StateSetManipulator(viewer.getCamera()->getOrCreateStateSet()));
			viewer.addEventHandler(new osgViewer::StatsHandler);
			viewer.setUpViewOnSingleScreen(1);//这里是单屏幕显示
			viewer.getCamera()->setClearColor(osg::Vec4(1, 1, 1, 0));
			
			viewer.run();
		}
	}
	return 0;
}
