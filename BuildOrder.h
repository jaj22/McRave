#include <BWAPI.h>
#include <vector>

using namespace std;
using namespace BWAPI;

// External unit count variables
extern int probeCnt;

// External building variables
extern int nexusDesired, nexusCnt;
extern int pylonDesired;
extern int gasDesired;
extern int gateDesired, gateCnt;
extern int forgeDesired, forgeCnt;
extern int coreDesired, coreCnt;

// External advanced building variables
extern int roboDesired, roboCnt;
extern int stargateDesired, stargateCnt;
extern int citadelDesired, citadelCnt;
extern int supportBayDesired, supportBayCnt;
extern int fleetBeaconDesired, fleetBeaconCnt;
extern int archivesDesired, archivesCnt;
extern int observatoryDesired, obseravtoryCnt;
extern int tribunalDesired, tribunalCnt;

// External resource and expansion variables
extern vector<TilePosition>gasTilePosition;
extern vector<int> mineralID, mineralWorkerID;
extern vector<TilePosition> nextExpansion;

// Function declaration
void getBuildOrder();