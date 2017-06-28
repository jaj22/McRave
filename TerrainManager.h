#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "Singleton.h"
#include "src\bwem.h"

using namespace BWAPI;
using namespace BWEM;
using namespace BWTA;
using namespace std;

namespace { auto & theMap = BWEM::Map::Instance(); }

class TerrainTrackerClass
{
	bool analyzed = false;
	bool wallin = false;
	bool analyzeHome = true;
	bool analyzeMap = true;
	bool BWTAdone = false;
	int currentSize = 0;

	CPPath path;
	set <int> allyTerritory;
	set <int> islandRegions;
	vector<Position> defendHere;
	set<Position> enemyBasePositions;
	set<TilePosition> allBaseLocations;
	Position enemyStartingPosition, playerStartingPosition, playerNaturalPosition;
	TilePosition enemyStartingTilePosition, playerStartingTilePosition, playerNaturalTilePosition;

public:
	void update();
	void setAnalyzed() { analyzed = true; }
	void setBWTAdone() { BWTAdone = true; }
	bool isAnalyzed() { return analyzed; }
	bool isBWTAdone() { return BWTAdone; }
	void removeTerritory(Unit);
	
	CPPath getPath() { return path; }
	set <int>& getAllyTerritory() { return allyTerritory; }	
	set <int>& getIslandRegions() { return islandRegions; }
	vector<Position>& getDefendHere() { return defendHere; }
	set<Position>& getEnemyBasePositions() { return enemyBasePositions; }
	set<TilePosition>& getAllBaseLocations() { return allBaseLocations; }

	Position getEnemyStartingPosition() { return enemyStartingPosition; }
	Position getPlayerStartingPosition() { return playerStartingPosition; }
	Position getPlayerNaturalPosition() { return playerNaturalPosition; }
	TilePosition getEnemyStartingTilePosition() { return enemyStartingTilePosition; }
	TilePosition getPlayerStartingTilePosition() { return playerStartingTilePosition; }	
	TilePosition getPlayerNaturalTilePosition() { return playerNaturalTilePosition; }

};

typedef Singleton<TerrainTrackerClass> TerrainTracker;