#pragma once
#include <BWAPI.h>

using namespace BWAPI;
using namespace std;

class PlayerInfo
{
	Race race;
	bool alive;
	set <UpgradeType> playerUpgrades;
	set <TechType> playerTechs;
public:
	PlayerInfo();
	Race getRace() { return race; }
	bool isAlive() { return alive; }

	void setRace(Race newRace) { race = newRace; }
	void setAlive(bool newState) { alive = newState; }
	void hasUpgrade(UpgradeType);
	void hasTech(TechType);
};