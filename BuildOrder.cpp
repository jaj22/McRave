#include "BuildOrder.h"

using namespace BWAPI;
int gateNum;

// Building consistency order: nexus, pylon, gas, gate, forge, core, robo, stargate, citadel, support, fleet, archives, observatory, tribunal

void getBuildOrder()
{	
	// Supply and expansions
	pylonDesired = min(22, (int)floor((Broodwar->self()->supplyUsed() / 14)));	

	// If we just attacked, expand
	if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Reaver) > 0 && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Shuttle) > 0)
	{
		firstAttack = 1;
	}

	// If we are teching up, ignore build order changes
	if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Cybernetics_Core) > 0)
	{
		fourPool = false;
		twoGate = false;
		twoRax = false;
	}

	// Gateways
	if (fourPool || twoGate || twoRax || enemyBasePositions.size() < 1)
	{
		if (Broodwar->self()->supplyUsed() >= 18 && Broodwar->self()->supplyUsed() < 26)
		{
			gateDesired = Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus);
		}
		else if (Broodwar->self()->supplyUsed() >= 26 && Broodwar->self()->supplyUsed() <= 70)
		{
			gateDesired = Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) + 1;
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
	else
	{		
		if (Broodwar->self()->supplyUsed() >= 18 && Broodwar->self()->supplyUsed() < 26)
		{
			gateDesired = Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus);
		}
		else if (Broodwar->self()->supplyUsed() >= 26 && Broodwar->self()->supplyUsed() <= 70)
		{
			gateDesired = Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) + Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Cybernetics_Core);
		}
		else if (Broodwar->self()->supplyUsed() >= 70)
		{
			gateDesired = Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) + Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Cybernetics_Core) + 1;		
		}
		else
		{
			gateDesired = 0;
		}
	}	

	switch (Broodwar->enemy()->getRace())
	{
	case Races::Enum::Zerg:	
		myBuilds(1);	
		break;
	case Races::Enum::Terran:
		if (twoRax)
		{
			myBuilds(0);
		}
		else
		{
			myBuilds(2);
		}
		break;
	case Races::Enum::Protoss:
		if (twoGate || enemyBasePositions.size() < 1)
		{
			myBuilds(0);
		}
		else
		{
			myBuilds(1);
		}
		break;
	case Races::Enum::Random:
		{
			myBuilds(0);
		}
		break;
	}
}

void myBuilds(int whichBuild)
{
	switch (whichBuild){
	case 0: // Temporary Defensive Build
		gateNum = 1;
		forgeDesired = min(1, ((int)floor(Broodwar->self()->supplyUsed() / 160)));
		coreDesired = min(1, (int)floor(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot) / 4));
		gasDesired = min((int)gasTilePosition.size(), (int)floor(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot) / 4));		
		break;		
	case 1: 
		// 3 Gate Robo into HT		
		gasDesired = min(Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Gateway), (int)gasTilePosition.size());
		forgeDesired = min(1, ((int)floor(Broodwar->self()->supplyUsed() / 160)));
		nexusDesired = min(3, 1 + (int)floor(Broodwar->self()->supplyUsed() / 120));

		// First Base Tech - Goons and Reavers
		coreDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Gateway));
		if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Dragoon) > 4 && Broodwar->self()->supplyUsed() < 160)
		{
			roboDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus));
		}
		if (Broodwar->self()->supplyUsed() >= 160)
		{
			roboDesired = min(2, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus));
		}
		supportBayDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Observatory));
		observatoryDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Robotics_Facility));
		
		// Second Base Tech - Speedlots and High Templars
		citadelDesired = min(1, max(0, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) - 1));
		archivesDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Citadel_of_Adun));		
		break;
	case 2: 
		// 2 Base Arbiter
		gateNum = 1;
		gasDesired = min(Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Gateway), (int)gasTilePosition.size());
		forgeDesired = min(1, ((int)floor(Broodwar->self()->supplyUsed() / 160)));
		nexusDesired = min(3, 1 + (int)floor(Broodwar->self()->supplyUsed() / 100));

		// First Base Tech - Goons and Reavers
		coreDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Gateway));
		if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Dragoon) > 4 && Broodwar->self()->supplyUsed() < 160)
		{
			roboDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus));
		}
		if (Broodwar->self()->supplyUsed() >= 160)
		{
			roboDesired = min(2, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus));
		}
		supportBayDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Observatory));
		observatoryDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Robotics_Facility));

		// Second Base Tech	- Speedlots and Arbiters		
		stargateDesired = min(1, max(0, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) - 1));
		
		tribunalDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Templar_Archives));
		citadelDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Stargate));	
		archivesDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Citadel_of_Adun));
		break;
	}
}