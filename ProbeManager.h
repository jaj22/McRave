#include <BWAPI.h>

using namespace std;
using namespace BWAPI;

class ResourceInfo
{
private:
	int gathererCount, remainingResources;
	Position resourcePosition;
	UnitType unitType;
public:
	// Constructors
	ResourceInfo();
	~ResourceInfo();
	ResourceInfo(int, int, Position, UnitType);

	// Accessors
	int getGathererCount() const;
	int getRemainingResources() const;
	Position getPosition() const;
	UnitType getUnitType() const;

	// Mutators
	void setGathererCount(int newGathererCount);
	void setRemainingResources(int newRemainingResources);
	void setPosition(Position newPosition);
	void setUnitType(UnitType newUnitType);
};
class ProbeInfo{
private:
	Unit target;
public:
	// Constructors
	ProbeInfo();
	~ProbeInfo();
	ProbeInfo(Unit target);

	// Accessors
	Unit getTarget() const;

	// Mutators
	void setTarget(Unit newTarget);
};

void storeMineral(Unit resource, map <Unit, ResourceInfo>& myMinerals);
void storeGas(Unit resource, map <Unit, ResourceInfo>& myGas);
void storeProbe(Unit probe, map <Unit, ProbeInfo>& myProbes);

// External Variables
extern vector<Unit> combatProbe;
extern bool saturated, gasNeeded;
extern map <Unit, ResourceInfo> myMinerals;
extern map <Unit, ResourceInfo> myGas;


// Functions
Unit assignProbe(Unit probe);
void assignCombat(Unit probe);
void unAssignCombat(Unit probe);