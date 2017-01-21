#include <BWAPI.h>
#include <vector>

using namespace BWAPI;
using namespace std;

// External structure count for pre-requisite buildings and expanding
extern int nexusCnt, nexusDesired, coreCnt, archivesCnt, fleetBeaconCnt;
extern vector <TilePosition> activeExpansion;
extern vector<int> buildingWorkerID;
extern TilePosition buildTilePosition;
extern Position buildPosition;
extern UnitType currentBuilding;

// External unit variables
extern int zealotCnt, dragoonCnt, darkTemplarCnt;

// External resource variables
extern int queuedMineral;
extern int queuedGas;

// External other variables
extern vector<TilePosition> nextExpansion;

// Function declarations
void buildingManager(UnitType building, Unit builder);
void nexusManager(UnitType building, Unit builder, TilePosition expansion);
void productionManager(Unit building);
bool canBuildHere(UnitType building, Unit builder, TilePosition buildTilePosition);
TilePosition getBuildLocationNear(UnitType building, Unit builder, TilePosition buildTilePosition);