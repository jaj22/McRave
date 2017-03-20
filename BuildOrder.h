#include <BWAPI.h>
#include <vector>

using namespace std;
using namespace BWAPI;

// External building variables
extern int nexusDesired, pylonDesired, gasDesired, gateDesired, forgeDesired, batteryDesired, coreDesired, roboDesired, stargateDesired, citadelDesired, supportBayDesired, fleetBeaconDesired, archivesDesired, observatoryDesired, tribunalDesired;
extern map <UnitType, int> buildingDesired;

// External resource and expansion variables
extern vector<TilePosition>gasTilePosition;
extern vector<Unit> geysers;
extern vector<TilePosition> nextExpansion;
extern vector<Position>enemyBasePositions;

// External expansion paramaters
extern int forceExpand;
extern int inactiveNexusCnt;
extern bool saturated;

// External enemy build tracking
extern bool enemyAggresion, scouting;
extern string currentStrategy;

// Function declaration
void getBuildOrder();
void myBuilds(int whichBuild);
void myOpeners(int whichOpener);