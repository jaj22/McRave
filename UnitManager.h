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
extern Position playerStartingPosition;
extern Position holdingPosition;
extern Position zealotPosition;

extern int nexusDesired;
extern int firstAttack;

extern double threatArray[256][256];
extern double enemyStrength, allyStrength;

// Shuttle ID and Reaver ID pairing
extern vector<int> shuttleID;
extern vector<int> reaverID;

// Internal position variables
extern Position fleePosition;
extern vector<Position> fleePositions;
extern vector<Position> unitsFleeing;

// Other
extern bool forceEngage;
extern int currentSize;
extern vector<BWTA::Region*> allyTerritory;
extern vector<Position> defendHere;
extern int enemyCountNearby;
extern int defendingUnitCount;

// Function declarations
void unitGetCommand(Unit unit);
void unitMicro(Unit unit);
double unitGetStrength(Unit unit);
Position unitRegroup(Unit unit);
Position unitFlee(Unit unit, Unit currentTarget);
void carrierManager(Unit unit);
void shuttleManager(Unit unit);
void shuttleHarass(Unit unit);
void observerManager(Unit unit);
void reaverManager(Unit unit);
int commandManager();