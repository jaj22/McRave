#include "BuildOrder.h"

using namespace BWAPI;

// Building consistency order: nexus, pylon, gas, gate, forge, core, robo, stargate, citadel, support, fleet, archives, observatory, tribunal
void getBuildOrder()
{
	pylonDesired = min(22, (int)floor((Broodwar->self()->supplyUsed() / 14)));
	nexusDesired = max(1, 1 + (int)floor(Broodwar->self()->supplyUsed() / 200) + firstAttack);

	// Gateways
	if (Broodwar->self()->supplyUsed() >= 18 && Broodwar->self()->supplyUsed() < 22)
	{
		gateDesired = Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus);
	}
	else if (Broodwar->self()->supplyUsed() >= 22 && Broodwar->self()->supplyUsed() <= 50)
	{
		gateDesired = Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) + 1;
	}
	else if (Broodwar->self()->supplyUsed() > 60)
	{
		gateDesired = Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) + 2;
	}
	else
	{
		gateDesired = 0;
	}


	// Assimilators
	if (Broodwar->self()->gas() < Broodwar->self()->minerals())
	{
		gasDesired = min((int)gasTilePosition.size(), (int)floor(Broodwar->self()->supplyUsed() / 24));
	}

	switch (Broodwar->enemy()->getRace())
	{
	case Races::Enum::Zerg:
		// Structures
		// Build 1: Counter hydra/ling/lurker using zealot/goon/reaver
		if (fourPool == true)
		{			
			forgeDesired = min(1, ((int)floor(Broodwar->self()->supplyUsed() / 160)));
			coreDesired = min(1, (int)floor(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot) / 4));
			gasDesired = min((int)gasTilePosition.size(), (int)floor(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot) / 4));
			roboDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Cybernetics_Core)*Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus));
			supportBayDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Observatory));
			observatoryDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Robotics_Facility));
			citadelDesired = min(1, max(0, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) - 2));
			archivesDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Citadel_of_Adun));
		}
		else
		{
			forgeDesired = min(1, ((int)floor(Broodwar->self()->supplyUsed() / 160)));
			coreDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Gateway) / 2);
			roboDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Cybernetics_Core)*Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus));
			supportBayDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Observatory));
			observatoryDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Robotics_Facility));
			citadelDesired = min(1, max(0, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) - 2));
			archivesDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Citadel_of_Adun));
		}
		
		
		// Build 2: Counter 4pool

		// Content WIP
		break;
	case Races::Enum::Terran:
		// Structures
		// Build 1: Counter bio using zealot/goon/DT/carrier
		if (twoRax)
		{
			forgeDesired = min(1, ((int)floor(Broodwar->self()->supplyUsed() / 160)));
			coreDesired = min(1, (int)floor(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot) / 4));
			gasDesired = min((int)gasTilePosition.size(), (int)floor(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot) / 4));
			roboDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Cybernetics_Core)*Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus));
			supportBayDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Observatory));
			observatoryDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Robotics_Facility));
			citadelDesired = min(1, max(0, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) - 2));
			archivesDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Citadel_of_Adun));
		}
		else
		{
			forgeDesired = min(1, ((int)floor(Broodwar->self()->supplyUsed() / 160)));
			coreDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Gateway) / 2);
			roboDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Cybernetics_Core)*Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus));
			supportBayDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Observatory));
			observatoryDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Robotics_Facility));
			citadelDesired = min(1, max(0, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) - 2));
			archivesDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Citadel_of_Adun));
		}
		// Build 2: Counter mech
		// Content WIP
		break;
	case Races::Enum::Protoss:
		// Counter 2 gate pressure
		if (twoGate)
		{			
			forgeDesired = min(1, ((int)floor(Broodwar->self()->supplyUsed() / 160)));
			coreDesired = min(1, (int)floor(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot) / 4));
			gasDesired = min((int)gasTilePosition.size(), (int)floor(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot) / 4));
			roboDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Cybernetics_Core)*Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus));
			supportBayDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Observatory));
			observatoryDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Robotics_Facility));
			citadelDesired = min(1, max(0, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) - 2));
			archivesDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Citadel_of_Adun));
		}
		else
		{
			forgeDesired = min(1, ((int)floor(Broodwar->self()->supplyUsed() / 160)));
			coreDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Gateway) / 2);			
			roboDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Cybernetics_Core)*Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus));
			supportBayDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Observatory));
			observatoryDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Robotics_Facility));
			citadelDesired = min(1, max(0, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) - 2));
			archivesDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Citadel_of_Adun));
		}
		break;
	}
}



//forgeDesired = min(2, 2 * ((int)floor(Broodwar->self()->supplyUsed() / 160)));
//gasDesired = min((int)gasTilePosition.size(), (int)floor(Broodwar->self()->supplyUsed() / 24));
//coreDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Gateway));
//citadelDesired = min(1, max(0, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) - 2));
//archivesDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Citadel_of_Adun));
//stargateDesired = min(3, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Cybernetics_Core)*Broodwar->self()->supplyUsed() / 130);
//fleetBeaconDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Stargate));