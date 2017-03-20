#include <BWAPI.h>
#include <BWTA.h>
#include <vector>
#include "src\bwem.h"

using namespace BWAPI;
using namespace std;
using namespace BWEM;

// External position variables
extern BWEM::CPPath path;
extern vector <Position> enemyBasePositions;
extern vector <TilePosition> nextExpansion;
extern Position enemyStartingPosition;
extern TilePosition enemyStartingTilePosition;
extern Position playerStartingPosition;
extern TilePosition playerStartingTilePosition;
extern Position supportPosition;

// External heatmaps and strength
extern double enemyHeatmap[256][256];
extern double airEnemyHeatmap[256][256];
extern int shuttleHeatmap[256][256];
extern int clusterHeatmap[256][256];
extern double enemyStrength, allyStrength;

// Shuttle ID and Reaver ID pairing
extern vector<int> shuttleID;
extern vector<int> harassShuttleID;
extern vector<int> reaverID;
extern vector<int> harassReaverID;

// Other
extern vector<BWTA::Region*> allyTerritory;
extern vector<Position> defendHere;
extern map <int, double> localEnemy;
extern map <int, double> localAlly;
extern map <int, int> unitRadiusCheck;
extern map <int, Position> unitsCurrentTarget;
extern map <Unit, int> unitsCurrentLocalCommand;
extern Color playerColor;
extern bool outsideBase;
extern vector<Unit> combatProbe;
extern int aSmall, aMedium, aLarge, eSmall, eMedium, eLarge;
extern bool enemyAggresion;

// Function declarations
void unitGetCommand(Unit unit);
void unitMicro(Unit unit);
double unitGetStrength(UnitType unitType);
double unitGetAirStrength(UnitType unitType);
double unitGetVisibleStrength(Unit unit);
double unitDamageMod(UnitType ally, UnitType enemy);
Position unitRegroup(Unit unit);
Position unitFlee(Unit unit, Unit currentTarget);
void shuttleManager(Unit unit);
void shuttleHarass(Unit unit);
void observerManager(Unit unit);
void reaverManager(Unit unit);
void carrierManager(Unit unit);
void corsairManager(Unit unit);
void templarManager(Unit unit);
void arbiterManager(Unit unit);
int unitGetGlobalStrategy();
Unit getTarget(Unit unit);
Unit getClusterTarget(Unit unit);

// Classes for enemy building tracking

class UnitInfo{
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

extern map <int, UnitInfo> enemyUnits;
int storeEnemyUnit(Unit building, map<int, UnitInfo>& enemyUnits);
