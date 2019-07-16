#pragma once

#include <Repository.h>
#include "Models/Npc.h"

class ClientRepository : public Repository
{
public:
	std::vector<Npc*>* ListNpcs();
};