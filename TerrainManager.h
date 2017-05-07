#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "src\bwem.h"
#include "Singleton.h"

using namespace BWAPI;
using namespace std;
using namespace BWTA;

namespace { auto & theMap = BWEM::Map::Instance(); }

class TerrainTrackerClass
{
	bool analyzed;
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