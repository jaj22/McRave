#include <BWAPI.h>
#include <BWTA.h>

using namespace BWAPI;
using namespace BWTA;
using namespace std;

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