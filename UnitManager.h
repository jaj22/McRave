#include <BWAPI.h>
#include <BWTA.h>
#include "src\bwem.h"
#include "Singleton.h"
#include "UnitInfo.h"

using namespace BWAPI;
using namespace std;
using namespace BWEM;

class UnitTrackerClass
{
	map <Unit, UnitInfoClass> enemyUnits;
	map <Unit, UnitInfoClass> allyUnits;
public:
	map<Unit, UnitInfoClass> getMyUnits() { return allyUnits; }
	map<Unit, UnitInfoClass> getEnUnits() { return enemyUnits; }

	void update();

	// Command manager
	void unitMicroTarget(Unit, Unit);
	void unitExploreArea(Unit);
	void unitGetCommand(Unit);
	void commandUpdate();

	// Strategy manager
	int unitGetGlobalStrategy();
	void unitGetLocalStrategy(Unit, Unit);

	// Target manager
	void unitGetTarget(Unit);
	void unitGetClusterTarget(Unit);
	void unitDeath(Unit);	

	// Special units
	void templarManager(Unit);
	void reaverManager(Unit);
	void arbiterManager(Unit);

	void storeEnemyUnit(Unit, map<Unit, UnitInfoClass>&);
	void storeAllyUnit(Unit, map<Unit, UnitInfoClass>&);

	Position unitFlee(Unit unit, Unit currentTarget);
};

// Terrain variables
extern BWEM::CPPath path;
extern TilePosition playerStartingTilePosition;
extern Position playerStartingPosition;
extern Position enemyStartingPosition;
extern set <BWTA::Region*> allyTerritory;
extern vector <Position> enemyBasePositions;
extern vector <Position> defendHere;
extern vector <TilePosition> nextExpansion;

// Heatmaps and Strength
extern double enemyGroundStrengthGrid[256][256];
extern double enemyAirStrengthGrid[256][256];
extern int shuttleHeatmap[256][256];
extern int enemyGroundClusterGrid[256][256];
extern int enemyAirClusterGrid[256][256];
extern int tankClusterHeatmap[256][256];
extern double enemyStrength, allyStrength;

// Unit Variables
extern Position supportPosition;
extern int aSmall, aMedium, aLarge, eSmall, eMedium, eLarge;

// Strategy Variables
extern bool outsideBase;
extern int forceExpand;

// Miscellaneous
extern int supply;

typedef Singleton<UnitTrackerClass> UnitTracker;