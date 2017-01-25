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
int probeCnt = 0, zealotCnt = 0, dragoonCnt = 0, highTemplarCnt = 0, darkTemplarCnt = 0, reaverCnt = 0, archonCnt = 0, darkArchonCnt = 0;
int observerCnt = 0, shuttleCnt = 0, scoutCnt = 0, carrierCnt = 0, arbiterCnt = 0, corsairCnt = 0;
Position builderPosition;
Position scouterPosition;

// Building Variables
int queuedMineral, queuedGas = 0;
int nexusCnt = 0, nexusBuildingCnt = 0, nexusDesired = 0, inactiveNexusCnt = 0;
int pylonDesired, pylonBuildingCnt = 0, pylonCnt = 0;
int	gasCnt = 0, gasBuildingCnt = 0, gasDesired = 0;
int gateDesired, gateBuildingCnt = 0, gateCnt = 0;
int forgeCnt = 0, forgeBuildingCnt = 0, forgeDesired = 0;
int coreCnt = 0, coreBuildingCnt = 0, coreDesired = 0;

// Advanced Building Variables
int roboCnt = 0, roboBuildingCnt = 0, roboDesired = 0;
int stargateCnt = 0, stargateBuildingCnt = 0, stargateDesired = 0;
int citadelCnt = 0, citadelBuildingCnt = 0, citadelDesired = 0;
int supportBayCnt = 0, supportBayBuildingCnt = 0, supportBayDesired = 0;
int fleetBeaconCnt = 0, fleetBeaconBuildingCnt = 0, fleetBeaconDesired = 0;
int archivesCnt = 0, archivesBuildingCnt = 0, archivesDesired = 0;
int observatoryCnt = 0, observatoryBuildingCnt = 0, observatoryDesired = 0;
int tribunalCnt = 0, tribunalBuildingCnt = 0, tribunalDesired = 0;

// Resource IDs
Position nexusPosition;
TilePosition nexusTilePosition;
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

// Enemy unit tracking
int enemySupply = 0;

// Base positions and tilepositions, used to find closest bases
TilePosition buildTilePosition; 
Position buildPosition;
UnitType currentBuilding;

// Holding positions
Position holdingPosition;
Position zealotPosition;

// Base positions
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

// Defensive building
TilePosition pylonNeeded;
TilePosition cannonNeeded;

// Variables that are unsorted below this line