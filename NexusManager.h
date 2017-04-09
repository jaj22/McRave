#include <BWAPI.h>

using namespace BWAPI;
using namespace std;

class NexusInfo{
private:
	int staticD;
public:
	NexusInfo();
	~NexusInfo();
	NexusInfo(int);
	int getStaticD() const;
	void setStaticD(int newStaticD);
};

extern map <Unit, NexusInfo> myNexus;
void updateDefenses(Unit nexus, map <Unit, NexusInfo>& myNexus);