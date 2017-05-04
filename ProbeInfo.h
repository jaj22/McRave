#include <BWAPI.h>

using namespace std;
using namespace BWAPI;

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