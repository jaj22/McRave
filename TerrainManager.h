#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "Singleton.h"
#include "src\bwem.h"

using namespace BWAPI;
using namespace BWTA;
using namespace std;

namespace { auto & theMap = BWEM::Map::Instance(); }

class TerrainTrackerClass
{
	bool analyzed = false;
	bool wallin = false;
	bool analyzeHome = true;
	bool analyzeMap = true;
	int currentSize = 0;
	BWEM::CPPath path;
	set <BWTA::Region*> territory;
	set <BWTA::Region*> allyTerritory;
	vector<BWTA::Region*> enemyTerritory;
	vector<Position> defendHere;
	vector<Position> enemyBasePositions;
	vector<TilePosition> nextExpansion;
	vector<TilePosition> activeExpansion;
	Position enemyStartingPosition, playerStartingPosition;
	TilePosition enemyStartingTilePosition, playerStartingTilePosition;
public:
	void update();
	void setAnalyzed();
	bool getAnalyzed() { return analyzed; }
	bool isWalled() { return wallin; }
	void setWalled() { wallin = true; }
	
	BWEM::CPPath getPath() { return path; }
	set <BWTA::Region*> getTerritory() { return territory; }
	set <BWTA::Region*> getAllyTerritory() { return allyTerritory; }
	vector<BWTA::Region*> getEnemyTerritory() { return enemyTerritory; }
	vector<Position> getDefendHere() { return defendHere; }
	vector<Position> getEnemyBasePositions() { return enemyBasePositions; }
	vector<TilePosition> getNextExpansion() { return nextExpansion; }
	vector<TilePosition> getActiveExpansion() { return activeExpansion; }
	Position getEnemyStartingPosition() { return enemyStartingPosition; }
	Position getPlayerStartingPosition() { return playerStartingPosition; }
	TilePosition getEnemyStartingTilePosition() { return enemyStartingTilePosition; }
	TilePosition getPlayerStartingTilePosition() { return playerStartingTilePosition; }	
};

typedef Singleton<TerrainTrackerClass> TerrainTracker;