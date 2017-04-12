#include <BWAPI.h>
#include <BWTA.h>
#include <vector>
#include "src\bwem.h"

using namespace BWAPI;
using namespace std;
using namespace BWEM;

// Class for enemy tracking
class UnitInfo {
	UnitType unitType, targetType;
	Position unitPosition, targetPosition;
	double unitStrength, unitLocal;
	UnitCommandType unitCommand;
	Unit target;
public:
	UnitInfo();
	UnitInfo(UnitType, Position, double, UnitCommandType);
	~UnitInfo();

	// Accessors
	UnitType getUnitType() const;
	Position getPosition() const;
	Position getTargetPosition() const;
	double getStrength() const;
	double getLocal() const;
	UnitCommandType getCommand() const;
	Unit getTarget() const;

	// Mutators
	void setUnitType(UnitType);
	void setPosition(Position);
	void setTargetPosition(Position);
	void setStrength(double);
	void setLocal(double);
	void setCommand(UnitCommandType);
	void setTarget(Unit);
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
extern map <Unit, UnitInfo> enemyUnits;
extern map <Unit, UnitInfo> allyUnits;
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
extern int supply;

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
int storeEnemyUnit(Unit unit, map<Unit, UnitInfo>& enemyUnits);
int storeAllyUnit(Unit unit, map<Unit, UnitInfo>& allyUnits);



