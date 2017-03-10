#include <BWAPI.h>
#include <vector>

using namespace std;
using namespace BWAPI;

// External building variables
extern int nexusDesired;
extern int pylonDesired;
extern int gasDesired;
extern int gateDesired;
extern int forgeDesired;
extern int batteryDesired;
extern int coreDesired;

// External advanced building variables
extern int roboDesired;
extern int stargateDesired;
extern int citadelDesired;
extern int supportBayDesired;
extern int fleetBeaconDesired;
extern int archivesDesired;
extern int observatoryDesired;
extern int tribunalDesired;

// External resource and expansion variables
extern vector<TilePosition>gasTilePosition;
extern vector<int> mineralID, mineralWorkerID;
extern vector<TilePosition> nextExpansion;
extern vector<Position>enemyBasePositions;

// External expansion paramaters
extern int forceExpand;


// External enemy build tracking
extern bool fourPool;
extern bool twoGate, twoRax;
extern bool scouting;
extern string currentStrategy;

// Function declaration
void getBuildOrder();
void myBuilds(int whichBuild);
void myOpeners(int whichOpener);