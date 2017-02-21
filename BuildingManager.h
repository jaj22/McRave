#include <BWAPI.h>
#include <vector>

using namespace BWAPI;
using namespace std;

// External building positioning variables
extern vector <TilePosition> activeExpansion;
extern vector <TilePosition> nextExpansion;
extern UnitType currentBuilding;
extern TilePosition buildTilePosition;

// External unit variables
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