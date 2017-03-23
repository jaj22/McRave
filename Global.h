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
bool outsideBase = false;

// Building Tracker Variables
int queuedMineral = 0, queuedGas = 0, reservedMineral = 0, reservedGas = 0;
int nexusDesired = 0, inactiveNexusCnt = 0;
int pylonDesired = 0;
int	gasDesired = 0;
int gateDesired = 0;
int forgeDesired = 0;
int batteryDesired = 0;
int coreDesired = 0;
int roboDesired = 0;
int stargateDesired = 0;
int citadelDesired = 0;
int supportBayDesired = 0;
int fleetBeaconDesired = 0;
int archivesDesired = 0;
int observatoryDesired = 0;
int tribunalDesired = 0;

// Building Manager Variables
map <int, UnitType> idleBuildings;
map <int, TechType> idleTech;
map <int, UpgradeType> idleUpgrade;
map <int, UnitType> idleGates;
map <UnitType, int> buildingDesired;
map <UnitType, pair<TilePosition, Unit>> queuedBuildings;

// Probe Manager Variables
map <Unit, Unit> gasProbeMap;
map <Unit, Unit> mineralProbeMap;
vector<int> scoutWorkerID;
vector<Unit> combatProbe;
bool saturated = false;

// Resource Manager Variables
vector<TilePosition> gasTilePosition;
vector <Unit> geysers;
map <Unit, int> gasMap;
map <Unit, int> mineralMap;

// Unit Manager Variables
map <int, double> localEnemy;
map <int, double> localAlly;
map <int, int> unitRadiusCheck;
map <int, Position> unitsCurrentTarget;
map <int, UnitInfo> enemyUnits;
map <Unit, int> unitsCurrentLocalCommand;
vector<int> shuttleID;
vector<int> harassShuttleID;
vector<int> reaverID;
vector<int> harassReaverID;
int enemyCountNearby = 0;
int defendingUnitCount = 0;
int enemyScoutedLast = 0;
int aSmall = 0, aMedium = 0, aLarge = 0, eSmall = 0, eMedium = 0, eLarge = 0;
Position supportPosition;

// Strategy Variables
bool enemyAggresion = false;
int forceExpand = 0;
string currentStrategy;

// Heatmaps
double allyStrength = 0.0, enemyStrength = 0.0;
double allyHeatmap[256][256] = { { 0 } };
double enemyHeatmap[256][256] = { { 0 } };
double airEnemyHeatmap[256][256] = { { 0 } }; 
int shuttleHeatmap[256][256] = { { 0 } };
int clusterHeatmap[256][256] = { { 0 } };

// Terrain Variables
int currentSize = 0;
BWEM::CPPath path;
set <BWTA::Region*> territory;
vector<BWTA::Region*> allyTerritory;
vector<BWTA::Region> enemyTerritory;
vector<Position> defendHere;
vector<Position>enemyBasePositions;
vector<TilePosition> nextExpansion;
vector<TilePosition> activeExpansion;
Position enemyStartingPosition, playerStartingPosition;
TilePosition enemyStartingTilePosition, playerStartingTilePosition;
