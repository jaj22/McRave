#include <BWAPI.h>

using namespace BWAPI;
using namespace std;

class NexusInfo{
private:
	int staticD;
public:
	// Constructors
	NexusInfo();
	~NexusInfo();
	NexusInfo(int);
	
	// Accessors
	int getStaticD() const;

	// Mutators
	void setStaticD(int newStaticD);
};

void updateDefenses(Unit nexus, map <Unit, NexusInfo>& myNexus);