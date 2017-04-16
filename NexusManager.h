#include <BWAPI.h>

using namespace BWAPI;
using namespace std;

class NexusInfo{
private:
	int staticD;
	TilePosition staticP;
public:
	// Constructors
	NexusInfo();
	~NexusInfo();
	NexusInfo(int, TilePosition);
	
	// Accessors
	int getStaticD() const;
	TilePosition getStaticP() const;

	// Mutators
	void setStaticD(int newStaticD);
	void setStaticP(TilePosition newStaticP);
};

void updateDefenses(Unit nexus, map <Unit, NexusInfo>& myNexus);