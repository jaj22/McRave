#include <BWAPI.h>

using namespace BWAPI;
using namespace std;

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

void updateResources(Unit resource, map <Unit, ResourceInfo>& myResources);