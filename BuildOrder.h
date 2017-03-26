#include <BWAPI.h>
#include <vector>

using namespace std;
using namespace BWAPI;

// External building variables
extern int nexusDesired, pylonDesired, gasDesired, gateDesired, forgeDesired, batteryDesired, coreDesired, roboDesired, stargateDesired, citadelDesired, supportBayDesired, fleetBeaconDesired, archivesDesired, observatoryDesired, tribunalDesired;
extern map <UnitType, int> buildingDesired;
extern map <int, UnitType> idleGates;

// External resource and expansion variables
extern vector<Unit> geysers;
extern vector<TilePosition> nextExpansion;
extern vector<Position>enemyBasePositions;

// External expansion paramaters
extern int forceExpand, inactiveNexusCnt;
extern vector<TilePosition> activeExpansion;
extern bool saturated, gasNeeded;

// External enemy build tracking
extern bool enemyAggresion, scouting;
extern string currentStrategy;

// Function declaration
void getBuildOrder();
void earlyBuilds(int whichBuild);
void midBuilds(int whichBuild);
void lateBuilds(int whichBuild);
