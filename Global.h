// Include API files
#include <BWAPI.h>
#include <BWTA.h>

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

using namespace BWAPI;
using namespace std;
using namespace BWTA;

bool BWTAhandling = false;
bool scouting = false;
bool enemyFound = false;

// Unit Variables
Position builderPosition;
Position scouterPosition;

// Building Variables
int queuedMineral, queuedGas = 0;
int nexusDesired = 0, inactiveNexusCnt = 0;
int pylonDesired = 0;
int	gasDesired = 0;
int gateDesired = 0;
int forgeDesired = 0;
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
vector<TilePosition> gasTilePosition;
vector<int> assimilatorID;

// Probe ID and their assignments
vector<int> probeID;
vector<int> deadProbeID;
vector<int> gasWorkerID;
vector<int> mineralWorkerID;
vector<int> buildingWorkerID;
vector<int> scoutWorkerID;
vector<int> combatWorkerID;

// Enemy unit tracking
int enemySupply = 0;
int enemyCountNearby = 0;

// Threat array
int threatArray[256][256] = { { 0 } };

// Friendly unit tracking
int allySupply = 0;

// Base positions and tilepositions, used to find closest bases
TilePosition buildTilePosition; 
Position buildPosition;
UnitType currentBuilding;

// Holding positions
Position holdingPosition;
Position zealotPosition;
TilePosition furthestNexus;

// Base positions
vector<int> nexusDistances;
vector<Position> basePositions;
vector<TilePosition> baseTilePositions;
vector<double> baseDistances;
vector<double> baseDistancesBuffer;
vector<double> expansionStartDistance;
vector<double> expansionRawDistance;

vector<double> nearestBases;
vector<double> furthestBases;

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

// Chokepoints
vector<Position>chokepointPositions;
vector<TilePosition>chokepointTilePositions;
vector<double> chokepointDistances;
vector<double> lowestChokepointDistance;
vector<double> chokepointDistancesBuffer;
vector<Position> nearestChokepointPosition;

// Targeting
Position currentTargetPosition;
Position currentPosition;
Position chokepointWrap;
Position nextPosition;
Position fleePosition;

// Defensive building
TilePosition pylonNeeded;
TilePosition cannonNeeded;

// Variables that are unsorted below this line
