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
extern int queuedMineral, queuedGas, reservedMineral, reservedGas; 

// Function declarations
void buildingManager(UnitType building, Unit builder);
bool canBuildHere(UnitType building, Unit builder, TilePosition buildTilePosition);
TilePosition getBuildLocationNear(UnitType building, Unit builder, TilePosition buildTilePosition);
void nexusManager(UnitType building, Unit builder, TilePosition expansion);
void productionManager(Unit building);


// Classes for enemy building tracking

class BuildingInfo{
	UnitType unitType;
	Position position;
public:	
	BuildingInfo();
	BuildingInfo(UnitType, Position);
	~BuildingInfo();

	UnitType getUnitType() const;
		// getUnitType
		// @return UnitType - BWAPI type of unit
	Position getPosition() const;
		// getPosition
		// @return Position - BWAPI position of unit
	//void setUnitType(UnitType) const;
	//	// setUnitType
	//	// @param UnitType - BWAPI type of unit
	//void setPosition(Position) const;
	//	// setPosition
	//	// @param Position - BWAPI position of unit
};

extern map <int, BuildingInfo> enemyBuildings;
void storeEnemyBuilding(Unit building, map<int, BuildingInfo>& enemyBuildings);