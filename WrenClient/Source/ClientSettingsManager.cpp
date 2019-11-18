#include "stdafx.h"
#include "ClientSettingsManager.h"
#include <fstream>
#include "Game.h"

static const std::string UI_HOTBAR_ABILITIES_SETTING_NAME = "UIHotbarAbilities:";

ClientSettingsManager::ClientSettingsManager(const Game& game, const std::string& playerName)
	: game{ game },
	  fileName{ "ClientSettings_" + playerName + ".txt" }
{
}

void ClientSettingsManager::SaveClientSettings()
{
	std::ofstream outf{ fileName };

	std::vector<UIAbility*>& uiAbilities = game.hotbar->GetUIAbilities();
	std::string abilitiesString;
	for (auto i = 0; i < 10; i++)
	{
		auto uiAbility = uiAbilities.at(i);
		if (uiAbility)
			abilitiesString += std::to_string(uiAbility->GetAbility()->abilityId);
		else
			abilitiesString += "-1";

		if (i < 9)
			abilitiesString += ",";
	}
	outf << UI_HOTBAR_ABILITIES_SETTING_NAME << abilitiesString << std::endl;
}

void ClientSettingsManager::LoadClientSettings()
{
	std::ifstream inf{ fileName };

	if (!inf)
		return;

	while (inf)
	{
		std::string strInput;
		inf >> strInput;

		if (strInput.find(UI_HOTBAR_ABILITIES_SETTING_NAME) != std::string::npos)
		{
			auto val = strInput.substr(UI_HOTBAR_ABILITIES_SETTING_NAME.length());
			std::replace(val.begin(), val.end(), ',', ' ');

			std::stringstream ss(val);
			int temp;
			while (ss >> temp)
				uiAbilityIds.push_back(temp);
		}
	}
}

const std::vector<int>& ClientSettingsManager::GetUIAbilityIds() const
{
	return uiAbilityIds;
}
