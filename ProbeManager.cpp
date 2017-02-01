#include "ProbeManager.h"

using namespace BWAPI;
using namespace std;

void assignProbe(Unit probe)
{	
	// Assign as a combat worker if we are being attacked
	if (enemyCountNearby > (allySupply + combatWorkerID.size()) && find(combatWorkerID.begin(), combatWorkerID.end(), probe->getID()) == combatWorkerID.end())
	{
		combatWorkerID.push_back(probe->getID());
		probe->attack(probe->getClosestUnit(Filter::IsEnemy && !Filter::IsBuilding && !Filter::IsFlyer));
		return;
	}
	else if (enemyCountNearby < (allySupply + combatWorkerID.size()) && find(combatWorkerID.begin(), combatWorkerID.end(), probe->getID()) != combatWorkerID.end())
	{
		combatWorkerID.erase(find(combatWorkerID.begin(), combatWorkerID.end(), probe->getID()));
	}
	if (find(combatWorkerID.begin(), combatWorkerID.end(), probe->getID()) != combatWorkerID.end() && (!probe->isAttacking() || !probe->isAttackFrame()))
	{
		probe->attack(probe->getClosestUnit(Filter::IsEnemy && !Filter::IsBuilding && !Filter::IsFlyer));
		return;
	}
	
	// If probe ID not found, give it an assignment
	if (find(probeID.begin(), probeID.end(), probe->getID()) == probeID.end())
	{			
		// Assign as a gas worker if we need more
		if ((int)gasWorkerID.size() < assimilatorID.size() * 3 && Broodwar->self()->gas() < Broodwar->self()->minerals())
		{
			probeID.push_back(probe->getID());
			//Broodwar << "Probe " << probe->getID() << " is now gathering gas." << std::endl;
			gasWorkerID.push_back(probe->getID());
			probe->gather(Broodwar->getUnit(assimilatorID.at((int)floor((find(gasWorkerID.begin(), gasWorkerID.end(), probe->getID()) - gasWorkerID.begin()) / 3))));
		}
		// Assign as mineral worker if we need more
		else if ((int)mineralWorkerID.size() < (mineralID.size() * 2))
		{
			probeID.push_back(probe->getID());
			mineralWorkerID.push_back(probe->getID());
			//Broodwar << "Probe " << probe->getID() << " is now gathering minerals." << std::endl;
			if (Broodwar->getFrameCount() < 100)
			{
				probe->gather(Broodwar->getUnit(mineralID.at((find(mineralWorkerID.begin(), mineralWorkerID.end(), probe->getID()) - mineralWorkerID.begin()) * 2)));
			}
			else
			{
				probe->gather(Broodwar->getUnit(mineralID.at((int)floor((find(mineralWorkerID.begin(), mineralWorkerID.end(), probe->getID()) - mineralWorkerID.begin()) / 2))));
			}
		}	
		// The nexus sometimes trains an extra
		else
		{
			probeID.push_back(probe->getID());
		}		
	}
	// If probe ID is found, find out if idle and tell it to get back to work!
	else if ((std::find(probeID.begin(), probeID.end(), probe->getID()) != probeID.end()) && probe->isIdle())
	{
		// If probe was a mineral worker, find his mineral field pair
		if (find(mineralWorkerID.begin(), mineralWorkerID.end(), probe->getID()) != mineralWorkerID.end() && mineralWorkerID.size() <= mineralID.size() * 2)
		{
			probe->gather(Broodwar->getUnit(mineralID.at((int)floor((find(mineralWorkerID.begin(), mineralWorkerID.end(), probe->getID()) - mineralWorkerID.begin()) / 2))));
		}
		else if (find(gasWorkerID.begin(), gasWorkerID.end(), probe->getID()) != gasWorkerID.end())
		{
			probe->gather(Broodwar->getUnit(assimilatorID.at((int)floor((find(gasWorkerID.begin(), gasWorkerID.end(), probe->getID()) - gasWorkerID.begin()) / 3))));
		}		
		else
		{
			probe->gather(probe->getClosestUnit(Filter::IsMineralField));
		}		
	}	
} // End of worker unit commands

void assignScout(Unit probe)
{

}

void assignBuilder(Unit probe)
{

}

void assignCombat(Unit probe)
{

}