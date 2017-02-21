// Include API files
#include <BWAPI.h>
#include <BWTA.h>
#include "src\bwem.h"

// Include other source files
#include "BuildingManager.h"
#include "BuildOrder.h"
#include "ProbeManager.h"
#include "UnitManager.h"
#include "StrategyManager.h"

// Include standard libraries that are needed
#include <vector>
#include <iostream>
#include <Windows.h>
#include <set>
#include <vector>
#include <iterator>
#include <fstream>
#include <BWTA.h>

// Namespaces
using namespace BWAPI;
using namespace std;
using namespace BWTA;

bool BWTAhandling = false;
bool scouting = true;

// Building Variables
int queuedMineral, queuedGas = 0;
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

// Enemy unit tracking
int enemyCountNearby = 0;
int defendingUnitCount = 0;
int enemyScoutedLast = 0;

// Enemy build tracking
bool fourPool, twoGate, twoRax = false;
int enemyGate, enemyRax = 0;

// Threat calculations
double allyStrength = 0.0, enemyStrength = 0.0;
int threatArray[256][256] = { { 0 } };
int allySupply = 0, enemySupply = 0;

// Building Manager Variables
UnitType currentBuilding;
TilePosition buildTilePosition;

// Territory Variables
int currentSize = 0;
vector<Position> defendHere;
set <BWTA::Region*> territory;
vector<BWTA::Region*> allyTerritory;
vector<BWTA::Region> enemyTerritory;

// --------------------------------------------
// Variables that are unsorted: 
// --------------------------------------------

// Base positions
BWEM::CPPath path;
vector<Position> basePositions;
vector<TilePosition> baseTilePositions;
vector<double> baseDistances;
vector<double> baseDistancesBuffer;
vector<double> expansionStartDistance;
vector<double> expansionRawDistance;
vector<double> nearestBases;
vector<Position>nearestBasePositions;
vector<TilePosition>nearestBaseTilePositions;
vector<TilePosition>nearestBaseTilePositionsBuffer;
vector<Position>enemyBasePositions;
Position enemyStartingPosition;
TilePosition enemyStartingTilePosition;
vector<TilePosition> nextExpansion;
vector<TilePosition> activeExpansion;

// Starting locations
Position playerStartingPosition;
TilePosition playerStartingTilePosition;
vector<Position>startingLocationPositions;
vector<TilePosition>startingLocationTilePositions;

//Expanding
int firstAttack = 0;

