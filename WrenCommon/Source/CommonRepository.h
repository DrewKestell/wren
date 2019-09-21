#pragma once

#include "Repository.h"
#include "Models/StaticObject.h"

class CommonRepository : public Repository
{
public:
	std::vector<std::unique_ptr<StaticObject>> ListStaticObjects();
};