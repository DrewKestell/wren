#pragma once

#include "Repository.h"
#include "Models/StaticObject.h"

class CommonRepository : public Repository
{
public:
	std::vector<StaticObject*> ListStaticObjects();
};