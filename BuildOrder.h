#include <BWAPI.h>
#include <vector>

using namespace std;
using namespace BWAPI;

extern int nexusDesired, nexusCnt;
extern int pylonDesired;
extern int gasDesired;
extern int gateDesired, gateCnt;
extern int forgeDesired, forgeCnt;
extern int coreDesired, coreCnt;


extern int roboDesired, roboCnt;
extern int stargateDesired, stargateCnt;
extern int citadelDesired, citadelCnt;
extern int supportBayDesired, supportBayCnt;
extern int fleetBeaconDesired, fleetBeaconCnt;
extern int archivesDesired, archivesCnt;
extern int observatoryDesired, obseravtoryCnt;
extern int tribunalDesired, tribunalCnt;

extern vector<TilePosition>gasTilePosition;
extern vector<int> mineralID, mineralWorkerID;

void getBuildOrder();