#include <BWAPI.h>
#include <BWTA.h>
#include <vector>
using namespace BWAPI;
using namespace std;

// External position variables
extern vector <Position> enemyBasePositions;
extern vector <Position> nearestChokepointPosition;
extern vector <TilePosition> nextExpansion;
extern Position enemyStartingPosition;
extern Position holdingPosition;
extern Position zealotPosition;

extern int threatArray[256][256];
extern int enemySupply, allySupply;

// Internal position variables
extern Position currentTargetPosition;
extern Position currentPosition;
extern Position chokepointWrap;
extern Position nextPosition;
extern Position fleePosition;

// Function declarations
void unitGetCommand(Unit unit);
void unitGetTarget(Unit unit);
Position fleeTo(TilePosition desiredPosition);
void carrierManager(Unit unit);
void shuttleManager(Unit unit);
void observerManager(Unit unit);
void reaverManager(Unit unit);
int commandManager();