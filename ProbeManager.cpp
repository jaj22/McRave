#include "ProbeManager.h"

using namespace BWAPI;
using namespace std;

void assignCombat(Unit probe)
{
	if (find(combatProbe.begin(), combatProbe.end(), probe) == combatProbe.end())
	{
		combatProbe.push_back(probe);
	}	
}

void unAssignCombat(Unit probe)
{
	combatProbe.erase(find(combatProbe.begin(), combatProbe.end(), probe));
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
	// Only update gas probes if we really need gas (helps with expansion timing)
	if (Broodwar->self()->gas() * 2 < Broodwar->self()->minerals())
	{
		for (auto gas : gasMap)
		{
			if (gas.second < 3)
			{
				assignGas(probe, gas.first);
				gasMap[gas.first] = gas.second + 1;
				return;
			}
		}
	}

	while (cnt <= 2)
	{
		for (auto mineral : mineralMap)
		{
			// First round on minerals
			if (mineral.second < cnt)
			{
				saturated = false;
				assignMinerals(probe, mineral.first);
				mineralMap[mineral.first] = cnt;
				return;
			}
		}
		cnt++;
	}
	saturated = true;
	if (probe->isIdle())
	{
		probe->gather(probe->getClosestUnit(Filter::IsMineralField));
	}
	return;
}