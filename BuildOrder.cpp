#include "BuildOrder.h"

using namespace BWAPI;

// Building consistency order: nexus, pylon, gas, gate, forge, core, robo, stargate, citadel, support, fleet, archives, observatory, tribunal

void getBuildOrder()
{
	// Supply and expansions
	pylonDesired = min(22, (int)floor((Broodwar->self()->supplyUsed() / 14)));
	nexusDesired = max(1, 1 + (int)floor(Broodwar->self()->supplyUsed() / 200) + firstAttack);

	// If we just attacked, expand
	if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Reaver) > 0 && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Shuttle) > 0)
	{
		firstAttack = 1;
	}

	// Gateways
	if (fourPool || twoGate || twoRax)
	{
		if (Broodwar->self()->supplyUsed() >= 18 && Broodwar->self()->supplyUsed() < 22)
		{
			gateDesired = Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus);
		}
		else if (Broodwar->self()->supplyUsed() >= 22 && Broodwar->self()->supplyUsed() <= 70)
		{
			gateDesired = Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) + 1;
		}
		else if (Broodwar->self()->supplyUsed() > 70)
		{
			gateDesired = Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) + 2;
		}
		else
		{
			gateDesired = 0;
		}
	}
	else
	{
		if (Broodwar->self()->supplyUsed() >= 18 && Broodwar->self()->supplyUsed() < 22)
		{
			gateDesired = Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus);
		}
		else if (Broodwar->self()->supplyUsed() >= 22 && Broodwar->self()->supplyUsed() <= 70)
		{
			gateDesired = Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) + Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Cybernetics_Core);
		}
		else if (Broodwar->self()->supplyUsed() > 70)
		{
			gateDesired = Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) + Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Cybernetics_Core) + 1;
		}
		else
		{
			gateDesired = 0;
		}
	}

	// If we have at least 4 dragoons, time to tech up
	if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Dragoon) > 4)
	{
		roboDesired = min(2, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus));
	}

	// Assimilators
	if (Broodwar->self()->gas() < Broodwar->self()->minerals() && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Gateway) > 0)
	{
		gasDesired = gasTilePosition.size();
	}

	switch (Broodwar->enemy()->getRace())
	{
	case Races::Enum::Zerg:			
		if (fourPool == true)		
		{			
			myBuilds(0);
		}
		else
		{
			myBuilds(1);
		}	
		break;
	case Races::Enum::Terran:			
		if (twoRax)			
		{
			myBuilds(0);
		}
		else			
		{
			myBuilds(1);
		}
		break;
	case Races::Enum::Protoss:		
		if (twoRax)
		{
			myBuilds(0);
		}
		else
		{
			myBuilds(1);
		}
		break;
	}
}

void myBuilds(int whichBuild)
{
	switch (whichBuild){
	case 0:
		// Defensive
		forgeDesired = min(1, ((int)floor(Broodwar->self()->supplyUsed() / 160)));
		coreDesired = min(1, (int)floor(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot) / 4));
		gasDesired = min((int)gasTilePosition.size(), (int)floor(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot) / 4));
		supportBayDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Observatory));
		observatoryDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Robotics_Facility));
		citadelDesired = min(1, max(0, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) - 2));
		archivesDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Citadel_of_Adun));		
		break;
		// Default
	case 1:
		forgeDesired = min(1, ((int)floor(Broodwar->self()->supplyUsed() / 160)));
		coreDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Gateway));
		supportBayDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Observatory));
		observatoryDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Robotics_Facility));
		citadelDesired = min(1, max(0, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) - 2));
		archivesDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Citadel_of_Adun));
		break;
	}
}