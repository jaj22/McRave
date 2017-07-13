#pragma once
#include "McRave.h"
#include "PlayerInfo.h"
#include "Singleton.h"

class PlayerTrackerClass
{
	map <Player, PlayerInfo> thePlayers;
public:
	map <Player, PlayerInfo>& getPlayers() { return thePlayers; }

	void update();
};

typedef Singleton<PlayerTrackerClass> PlayerTracker;