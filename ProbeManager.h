#include <BWAPI.h>

using namespace std;
using namespace BWAPI;

class ResourceInfo
{
private:
	int gathererCount, remainingResources;
	Position resourcePosition;
public:
	// Constructors
	ResourceInfo();
	~ResourceInfo();
	ResourceInfo(int, int, Position);

	// Accessors
	int getGathererCount() const;
	int getRemainingResources() const;
	Position getPosition() const;

	// Mutators
	void setGathererCount(int newGathererCount);
	void setRemainingResources(int newRemainingResources);
	void setPosition(Position newPosition);
};

void storeMineral(Unit resource, map <Unit, ResourceInfo>& myMinerals);
void storeGas(Unit resource, map <Unit, ResourceInfo>& myGas);

// External Variables
extern vector<Unit> combatProbe;
extern map <Unit, pair<Unit, Position>> mineralProbeMap;
extern map <Unit, Unit> gasProbeMap;
extern map <Unit, int> mineralMap, gasMap;
extern bool saturated, gasNeeded;
extern map <Unit, ResourceInfo> myMinerals;
extern map <Unit, ResourceInfo> myGas;


// Functions
void assignProbe(Unit probe);
void assignCombat(Unit probe);
void unAssignCombat(Unit probe);