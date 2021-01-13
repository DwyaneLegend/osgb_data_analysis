#pragma once
#include "Triangle.h"
#include <vector>
#include <list>
using namespace std;
class Buildings {
public:
	list<Triangle*> building_triangles;
	list<int> belong;
	Buildings();
	~Buildings();
};