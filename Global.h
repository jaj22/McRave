// Include API files
#include <BWAPI.h>
#include <BWTA.h>
#include "src\bwem.h"
#include "Singleton.h"

// Include other source files
#include "BuildingManager.h"
#include "ProductionManager.h"
#include "BuildOrder.h"
#include "ResourceManager.h"
#include "ProbeManager.h"
#include "UnitManager.h"
#include "UnitScore.h"
#include "NexusManager.h"
#include "StrategyManager.h"
#include "GridManager.h"


// Include standard libraries that are needed
#include <vector>
#include <iostream>
#include <Windows.h>
#include <set>
#include <iterator>
#include <fstream>
#include <BWTA.h>
#include <utility>

// Namespaces
using namespace BWAPI;
using namespace std;
using namespace BWTA;

// Building Tracker Variables
int queuedMineral = 0, queuedGas = 0, reservedMineral = 0, reservedGas = 0;
int inactiveNexusCnt = 0;

// Building Manager Variables
map <Unit, NexusInfo> myNexus;
map <int, UnitType> idleBuildings;
map <int, TechType> idleTech;
map <int, UpgradeType> idleUpgrade;
map <int, UnitType> idleGates;
map <UnitType, int> buildingDesired;
map <UnitType, pair<TilePosition, Unit>> queuedBuildings;

// Probe Manager Variables
Unit scout;
bool saturated = false, gasNeeded = false;

// Resource Manager Variables
vector <Unit> geysers;

// Unit Manager Variables
int supply;
int aSmall = 0, aMedium = 0, aLarge = 0, eSmall = 0, eMedium = 0, eLarge = 0;
Position supportPosition;

// Strategy Variables
string currentStrategy;

// Terrain Variables
int currentSize = 0;
BWEM::CPPath path;
set <BWTA::Region*> territory;
set <BWTA::Region*> allyTerritory;
vector<BWTA::Region> enemyTerritory;
vector<Position> defendHere;
vector<Position> enemyBasePositions;
vector<TilePosition> nextExpansion;
vector<TilePosition> activeExpansion;
Position enemyStartingPosition, playerStartingPosition;
TilePosition enemyStartingTilePosition, playerStartingTilePosition;
