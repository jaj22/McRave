#include <BWAPI.h>
#include <vector>

using namespace BWAPI;
using namespace std;

// External structure count for pre-requisite buildings and expanding
extern int nexusCnt, nexusDesired, coreCnt, archivesCnt, fleetBeaconCnt;

// External building positioning variables
extern vector <TilePosition> activeExpansion;
extern vector <TilePosition> nextExpansion;
extern TilePosition buildTilePosition;
extern Position buildPosition;
extern UnitType currentBuilding;

// External unit variables
extern int probeCnt, zealotCnt, dragoonCnt, highTemplarCnt, darkTemplarCnt, reaverCnt, archonCnt, darkArchonCnt;
extern int observerCnt, shuttleCnt, scoutCnt, carrierCnt, arbiterCnt, corsairCnt;
extern vector<int> buildingWorkerID;

// External resource variables
extern int queuedMineral;
extern int queuedGas;

// Function declarations
void buildingManager(UnitType building, Unit builder);
bool canBuildHere(UnitType building, Unit builder, TilePosition buildTilePosition);
TilePosition getBuildLocationNear(UnitType building, Unit builder, TilePosition buildTilePosition);
void nexusManager(UnitType building, Unit builder, TilePosition expansion);
void productionManager(Unit building);