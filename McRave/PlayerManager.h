#pragma once
#include "McRave.h"
#include "PlayerInfo.h"
#include "Singleton.h"

class PlayerTrackerClass
{
	map <Player, PlayerInfo> thePlayers;
	int eZerg, eProtoss, eTerran;
public:
	map <Player, PlayerInfo>& getPlayers() { return thePlayers; }

	void update();
	int getNumberZerg() { return eZerg; }
	int getNumberProtoss() { return eProtoss; }
	int getNumberTerran() { return eTerran; }
};

typedef Singleton<PlayerTrackerClass> PlayerTracker;