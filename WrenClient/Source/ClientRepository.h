#pragma once

#include <Repository.h>
#include "Models/Npc.h"
#include "Models/Item.h"

class ClientRepository : public Repository
{
public:
	std::vector<std::unique_ptr<Npc>> ListNpcs();
	std::vector<std::unique_ptr<Item>> ListItems();
};