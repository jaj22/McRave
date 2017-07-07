#pragma once
#include <BWAPI.h>
#include "Singleton.h"

using namespace BWAPI;
using namespace BWEM;
using namespace std;

namespace { auto & theMap = BWEM::Map::Instance(); }

class TerrainTrackerClass
{
	CPPath path;
	set <int> allyTerritory;
	set <int> islandRegions;
	vector<Position> defendHere;
	set<Position> enemyBasePositions;
	set<TilePosition> allBaseLocations;
	Position enemyStartingPosition, playerStartingPosition;
	TilePosition enemyStartingTilePosition, playerStartingTilePosition, FFEPosition;
	TilePosition secondChoke, firstChoke;

public:
	void update();
	void removeTerritory(Unit);

	bool isInAllyTerritory(Unit);
	Position getClosestEnemyBase(Position);
	TilePosition getRandomBase();	
	
	CPPath getPath() { return path; }
	set <int>& getAllyTerritory() { return allyTerritory; }	
	set <int>& getIslandRegions() { return islandRegions; }
	vector<Position>& getDefendHere() { return defendHere; }
	set<Position>& getEnemyBasePositions() { return enemyBasePositions; }
	set<TilePosition>& getAllBaseLocations() { return allBaseLocations; }

	Position getEnemyStartingPosition() { return enemyStartingPosition; }
	Position getPlayerStartingPosition() { return playerStartingPosition; }
	TilePosition getEnemyStartingTilePosition() { return enemyStartingTilePosition; }
	TilePosition getPlayerStartingTilePosition() { return playerStartingTilePosition; }	
	TilePosition getFFEPosition() { return FFEPosition; }
	TilePosition getFirstChoke() { return firstChoke; }
	TilePosition getSecondChoke() { return secondChoke; }
};

typedef Singleton<TerrainTrackerClass> TerrainTracker;