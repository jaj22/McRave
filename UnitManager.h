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

	void unitMicroTarget(Unit, Unit);
	void unitExploreArea(Unit);

	int unitGetGlobalStrategy();
	void unitGetLocalStrategy(Unit, Unit);
	void unitGetCommand(Unit);
	void unitGetTarget(Unit);
	void unitGetClusterTarget(Unit);
	void unitUpdate(Unit);
	void unitDeath(Unit);

	// Command manager?
	void unitLocalWinCommand(Unit);
	void unitLocalLoseCommand(Unit);
	void unitGlobalWinCommand(Unit);
	void unitGlobalLoseCommand(Unit);

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
extern vector<Unit> combatProbe;
extern int supply;

// Special Unit Functions
void shuttleManager(Unit unit);
void shuttleHarass(Unit unit);
void observerManager(Unit unit);
void reaverManager(Unit unit);
void carrierManager(Unit unit);
void corsairManager(Unit unit);
void templarManager(Unit unit);
void arbiterManager(Unit unit);

// Unit Tracking Functions
void storeEnemyUnit(Unit unit, map<Unit, UnitInfoClass>& enemyUnits);
void storeAllyUnit(Unit unit, map<Unit, UnitInfoClass>& allyUnits);

typedef Singleton<UnitTrackerClass> UnitTracker;