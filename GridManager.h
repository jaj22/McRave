#include "Singleton.h"
#include <BWAPI.h>
#include <BWTA.h>

using namespace BWAPI;
using namespace BWTA;
using namespace std;

class GridTrackerClass
{
	double enemyGroundStrengthGrid[256][256];
	double enemyAirStrengthGrid[256][256];
	int shuttleHeatmap[256][256];
	int enemyGroundClusterGrid[256][256];
	int enemyAirClusterGrid[256][256];
	int tankClusterHeatmap[256][256];
	int allyClusterGrid[256][256];
	int allyDetectorGrid[256][256];
	int resourceGrid[256][256];
public:
	void update();
	void reset();
};