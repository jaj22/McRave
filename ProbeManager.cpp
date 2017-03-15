#include "ProbeManager.h"

using namespace BWAPI;
using namespace std;

void assignCombat(Unit probe)
{

}

void assignGas(Unit probe, Unit gas)
{
	gasProbeMap[probe] = gas;
	probe->gather(gas);
	return;
}

void assignMinerals(Unit probe, Unit mineral)
{
	mineralProbeMap[probe] = mineral;
	probe->gather(mineral);
	return;
}

void assignProbe(Unit probe)
{
	int cnt = 1;
	for (auto gas : gasMap)
	{
		if (gas.second < 3)
		{
			assignGas(probe, gas.first);
			gasMap[gas.first] = gas.second + 1;
			return;
		}
	}

	while (cnt <= 2)
	{
		for (auto mineral : mineralMap)
		{
			// First round on minerals
			if (mineral.second < cnt)
			{
				assignMinerals(probe, mineral.first);
				mineralMap[mineral.first] = cnt;
				return;
			}
		}
		cnt++;
	}	
	probe->gather(probe->getClosestUnit(Filter::IsMineralField));
}