#pragma once

#include <Repository.h>
#include "Models/Npc.h"

class ClientRepository : public Repository
{
public:
	std::vector<std::unique_ptr<Npc>> ListNpcs();
};