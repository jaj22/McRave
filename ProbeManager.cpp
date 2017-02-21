#include "ProbeManager.h"

using namespace BWAPI;
using namespace std;

void assignProbe(Unit probe)
{	
	// Assign as a combat worker if we are being attacked and need help
	if (probe->getHitPoints() >= 20 && enemyCountNearby > (allySupply + combatWorkerID.size()) && find(combatWorkerID.begin(), combatWorkerID.end(), probe->getID()) == combatWorkerID.end() && find(scoutWorkerID.begin(), scoutWorkerID.end(), probe->getID()) == scoutWorkerID.end())
	{
		combatWorkerID.push_back(probe->getID());
		probe->attack(probe->getClosestUnit(Filter::IsEnemy && !Filter::IsBuilding && !Filter::IsFlyer));
		return;
	}
	// If probe was a combat worker and not needed anymore, stop attacking, return to old assignment
	else if ((probe->getHitPoints() < 10 || enemyCountNearby < (allySupply + combatWorkerID.size())) && find(combatWorkerID.begin(), combatWorkerID.end(), probe->getID()) != combatWorkerID.end())
	{
		combatWorkerID.erase(find(combatWorkerID.begin(), combatWorkerID.end(), probe->getID()));
		probe->stop();
		return;
	}
	// If combat worker, attack closest enemy
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
			gasWorkerID.push_back(probe->getID());
			probe->gather(Broodwar->getUnit(assimilatorID.at((int)floor((find(gasWorkerID.begin(), gasWorkerID.end(), probe->getID()) - gasWorkerID.begin()) / 3))));
			return;
		}
		// Assign as mineral worker if we need more (2x mineral size)
		else if ((int)mineralWorkerID.size() < (mineralID.size() * 2))
		{
			probeID.push_back(probe->getID());
			mineralWorkerID.push_back(probe->getID());			
			if (Broodwar->getFrameCount() < 100)
			{
				probe->gather(Broodwar->getUnit(mineralID.at((find(mineralWorkerID.begin(), mineralWorkerID.end(), probe->getID()) - mineralWorkerID.begin()) * 2)));
				return;
			}
			else
			{
				probe->gather(Broodwar->getUnit(mineralID.at((int)floor((find(mineralWorkerID.begin(), mineralWorkerID.end(), probe->getID()) - mineralWorkerID.begin()) / 2))));
				return;
			}
		}	
		// Assign as additional mineral worker to reach saturation (extra 0.5x mineral size)
		else if ((int)additionalMineralWorkerID.size() < (int)mineralID.size() * 2.5)
		{
			probeID.push_back(probe->getID());
			additionalMineralWorkerID.push_back(probe->getID());			
			if (activeExpansion.size() > 0 && mineralID.size() > 0)
			{
				int a = floor((find(additionalMineralWorkerID.begin(), additionalMineralWorkerID.end(), probe->getID()) - additionalMineralWorkerID.begin()) / (((int)mineralID.size() * 2.5)+1));
				probe->gather(Broodwar->getClosestUnit(Position(activeExpansion.at(a).x*32, activeExpansion.at(a).y*32), Filter::IsMineralField));
				return;
			}
		}		
		else
		{
			probeID.push_back(probe->getID());
		}		
	}
	// If probe ID is found, find out if idle and find what assignment it has
	else if (find(probeID.begin(), probeID.end(), probe->getID()) != probeID.end() && probe->isIdle())
	{
		// If probe was a mineral worker, find his mineral field pair
		if (find(mineralWorkerID.begin(), mineralWorkerID.end(), probe->getID()) != mineralWorkerID.end() && mineralWorkerID.size() <= mineralID.size() * 2)
		{
			probe->gather(Broodwar->getUnit(mineralID.at((int)floor((find(mineralWorkerID.begin(), mineralWorkerID.end(), probe->getID()) - mineralWorkerID.begin()) / 2))));
			return;
		}
		// If probe was a gas worker, find his gas field pair
		else if (find(gasWorkerID.begin(), gasWorkerID.end(), probe->getID()) != gasWorkerID.end())
		{
			probe->gather(Broodwar->getUnit(assimilatorID.at((int)floor((find(gasWorkerID.begin(), gasWorkerID.end(), probe->getID()) - gasWorkerID.begin()) / 3))));
			return;
		}		
		// Else if no assignment, just mine minerals for now
		else
		{
			probe->gather(probe->getClosestUnit(Filter::IsMineralField));
			return;
		}		
	}	
}

void assignScout(Unit probe)
{

}

void assignBuilder(Unit probe)
{

}

void assignCombat(Unit probe)
{

}