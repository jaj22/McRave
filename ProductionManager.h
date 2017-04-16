#include <BWAPI.h>

using namespace BWAPI;
using namespace std;

extern map <int, UnitType> idleBuildings;
extern map <int, UnitType> idleGates;
extern map <int, TechType> idleTech;
extern map <int, UpgradeType> idleUpgrade;
extern int queuedMineral, queuedGas, reservedMineral, reservedGas;
extern int supply;
extern map <UnitType, double> unitScore;
extern bool noZealots;

void productionManager(Unit building);