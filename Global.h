// Include API files
#include <BWAPI.h>
#include <BWTA.h>
#include "src\bwem.h"

// Include other source files
#include "BuildingManager.h"
#include "BuildOrder.h"
#include "ProbeManager.h"
#include "UnitManager.h"

// Include standard libraries that are needed
#include <vector>
#include <iostream>
#include <Windows.h>
#include <set>
#include <vector>
#include <iterator>
#include <fstream>
#include <BWTA.h>
#include <utility>

// Namespaces
using namespace BWAPI;
using namespace std;
using namespace BWTA;


bool BWTAhandling = false;
bool scouting = true;
bool clearCut = false;

// Building Variables
int queuedMineral = 0, queuedGas = 0, reservedMineral = 0, reservedGas = 0;
int nexusDesired = 0, inactiveNexusCnt = 0;
int pylonDesired = 0;
int	gasDesired = 0;
int gateDesired = 0;
int forgeDesired = 0;
int batteryDesired = 0;
int coreDesired = 0;

// Advanced Building Variables
int roboDesired = 0;
int stargateDesired = 0;
int citadelDesired = 0;
int supportBayDesired = 0;
int fleetBeaconDesired = 0;
int archivesDesired = 0;
int observatoryDesired = 0;
int tribunalDesired = 0;

// Resource IDs
vector<int> mineralID;
vector<int> assimilatorID;
vector<TilePosition> gasTilePosition;

// Probe ID and their assignments
vector<int> probeID;
vector<int> deadProbeID;
vector<int> gasWorkerID;
vector<int> mineralWorkerID;
vector<int> additionalMineralWorkerID;
vector<int> buildingWorkerID;
vector<int> scoutWorkerID;
vector<int> combatWorkerID;

// Shuttle ID and Reaver ID pairing
vector<int> shuttleID;
vector<int> harassShuttleID;
vector<int> reaverID;
vector<int> harassReaverID;

// Enemy unit tracking
int enemyCountNearby = 0;
int defendingUnitCount = 0;
int enemyScoutedLast = 0;
map <int, UnitInfo> enemyUnits;

// Enemy build tracking
bool fourPool = false, twoGate = false, twoRax = false;

// Heatmaps
double allyStrength = 0.0, enemyStrength = 0.0;
double enemyHeatmap[256][256] = { { 0 } };
double allyHeatmap[256][256] = { { 0 } };
double airEnemyHeatmap[256][256] = { { 0 } }; 
int shuttleHeatmap[256][256] = { { 0 } };
int allySupply = 0, enemySupply = 0;

// Building Manager Variables
UnitType currentBuilding;
TilePosition buildTilePosition;
map <int, UnitType> idleBuildings;
map <int, TechType> idleTech;
map <int, UpgradeType> idleUpgrade;

// Territory Variables
int currentSize = 0;
vector<Position> defendHere;
set <BWTA::Region*> territory;
vector<BWTA::Region*> allyTerritory;
vector<BWTA::Region> enemyTerritory;
bool forceEngage = false;

// Base positions
Position enemyStartingPosition;
TilePosition enemyStartingTilePosition;
vector<Position>enemyBasePositions;
vector<TilePosition> nextExpansion;
vector<TilePosition> activeExpansion;

// Starting locations
Position playerStartingPosition;
TilePosition playerStartingTilePosition;

//Unsorted
map <int, TilePosition> testBases;
bool doOnce = true;
BWEM::CPPath path;
Position arbiterPosition;