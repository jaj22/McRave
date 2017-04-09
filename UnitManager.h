#include <BWAPI.h>
#include <BWTA.h>
#include <vector>
#include "src\bwem.h"

using namespace BWAPI;
using namespace std;
using namespace BWEM;

// Class for enemy tracking
class UnitInfo {
	UnitType unitType;
	Position position;
public:
	UnitInfo();
	UnitInfo(UnitType, Position);
	~UnitInfo();

	UnitType getUnitType() const;
	// getUnitType
	// @return UnitType - BWAPI type of unit
	Position getPosition() const;
	// getPosition
	// @return Position - BWAPI position of unit
	void setUnitType(UnitType);
	// setUnitType
	// @param UnitType - BWAPI type of unit
	void setPosition(Position);
	// setPosition
	// @param Position - BWAPI position of unit
};


// Terrain variables
extern BWEM::CPPath path;
extern TilePosition playerStartingTilePosition;
extern Position playerStartingPosition;
extern Position enemyStartingPosition;
extern vector<BWTA::Region*> allyTerritory;
extern vector <Position> enemyBasePositions;
extern vector <Position> defendHere;
extern vector <TilePosition> nextExpansion;

// Heatmaps and Strength
extern double enemyHeatmap[256][256];
extern double airEnemyHeatmap[256][256];
extern int shuttleHeatmap[256][256];
extern int clusterHeatmap[256][256];
extern int tankClusterHeatmap[256][256];
extern double enemyStrength, allyStrength;

// Unit Variables
extern map <int, UnitInfo> enemyUnits;
extern map <int, UnitInfo> allyUnits;
extern map <Unit, double> localEnemy;
extern map <Unit, double> localAlly;
extern map <Unit, int> unitRadiusCheck;
extern map <Unit, Position> unitsCurrentTarget;
extern map <Unit, int> unitsCurrentLocalCommand;
extern map <Unit, Position> invisibleUnits;
extern map <UnitType, map<UnitType, int>> unitTargets;
extern Position supportPosition;
extern int aSmall, aMedium, aLarge, eSmall, eMedium, eLarge;

// Shuttle ID and Reaver ID pairing
extern vector<int> shuttleID;
extern vector<int> harassShuttleID;
extern vector<int> reaverID;
extern vector<int> harassReaverID;

// Strategy Variables
extern bool outsideBase;
extern int forceExpand;
extern bool enemyAggresion;

// Miscellaneous
extern vector<Unit> combatProbe;

// Strategy Functions
void unitGetCommand(Unit unit);
double unitGetStrength(UnitType unitType);
double unitGetAirStrength(UnitType unitType);
double unitGetVisibleStrength(Unit unit);
double unitDamageMod(UnitType ally, UnitType enemy);
Position unitRegroup(Unit unit);
Position unitFlee(Unit unit, Unit currentTarget);

// Targeting Functions
Unit getTarget(Unit unit);
Unit getClusterTarget(Unit unit);

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
int storeEnemyUnit(Unit building, map<int, UnitInfo>& enemyUnits);
int storeAllyUnit(Unit unit, map<int, UnitInfo>& allyUnits);



