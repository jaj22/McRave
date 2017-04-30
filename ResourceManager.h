#include <BWAPI.h>

using namespace std;
using namespace BWAPI;

class ResourceInfo
{
private:
	int gathererCount, remainingResources;
	Position resourcePosition;
	TilePosition resourceTilePosition;
	UnitType unitType;
	Unit nexus;
public:
	// Constructors
	ResourceInfo();
	~ResourceInfo();
	ResourceInfo(int, int, Unit, Position, TilePosition, UnitType);

	// Accessors
	int getGathererCount() const;
	int getRemainingResources() const;
	Unit getClosestNexus() const;
	Position getPosition() const;
	TilePosition getTilePosition() const;
	UnitType getUnitType() const;

	// Mutators
	void setGathererCount(int newGathererCount);
	void setRemainingResources(int newRemainingResources);
	void setClosestNexus(Unit nexus);
	void setPosition(Position newPosition);
	void setTilePosition(TilePosition newTilePosition);
	void setUnitType(UnitType newUnitType);
};

void storeMineral(Unit resource, map <Unit, ResourceInfo>& myMinerals);
void storeGas(Unit resource, map <Unit, ResourceInfo>& myGas);
Unit assignResource(map <Unit, ResourceInfo>& myMinerals, map <Unit, ResourceInfo>& myGas);