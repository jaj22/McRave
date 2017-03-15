#include <BWAPI.h>
#include <vector>

using namespace BWAPI;
using namespace std;

// External building positioning variables
extern vector <TilePosition> activeExpansion;
extern vector <TilePosition> nextExpansion;



// External unit variables
extern vector<int> buildingWorkerID;

// External resource variables
extern int queuedMineral, queuedGas, reservedMineral, reservedGas;
extern vector<Unit> geysers;
extern map <int, UnitType> idleBuildings;
extern map <int, TechType> idleTech;
extern map <int, UpgradeType> idleUpgrade;

// Function declarations
TilePosition buildingManager(UnitType building);
bool canBuildHere(UnitType building, TilePosition buildTilePosition);
TilePosition getBuildLocationNear(UnitType building, TilePosition buildTilePosition);
void productionManager(Unit building);