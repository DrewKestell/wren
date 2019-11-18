#pragma once

class Game;

class ClientSettingsManager
{
	const Game& game;
	const std::string fileName;
	std::vector<int> uiAbilityIds;
public:
	ClientSettingsManager(const Game& game, const std::string& playerName);
	void SaveClientSettings();
	void LoadClientSettings();
	
	const std::vector<int>& GetUIAbilityIds() const;
};