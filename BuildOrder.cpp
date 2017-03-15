#include "BuildOrder.h"

using namespace BWAPI;
int gateNum = 0;
int secondExpand = 0;
bool getOpener = true;

// Building consistency order: nexus, pylon, gas, gate, forge, core, robo, stargate, citadel, support, fleet, archives, observatory, tribunal

void myBuildings()
{
	buildingDesired[UnitTypes::Protoss_Nexus] = nexusDesired;
	buildingDesired[UnitTypes::Protoss_Pylon] = pylonDesired;
	buildingDesired[UnitTypes::Protoss_Assimilator] = gasDesired;
	buildingDesired[UnitTypes::Protoss_Gateway] = gateDesired;
	buildingDesired[UnitTypes::Protoss_Forge] = forgeDesired;	
	buildingDesired[UnitTypes::Protoss_Cybernetics_Core] = coreDesired;
	buildingDesired[UnitTypes::Protoss_Robotics_Facility] = roboDesired;
	buildingDesired[UnitTypes::Protoss_Stargate] = stargateDesired;
	buildingDesired[UnitTypes::Protoss_Citadel_of_Adun] = citadelDesired;
	buildingDesired[UnitTypes::Protoss_Robotics_Support_Bay] = supportBayDesired;
	buildingDesired[UnitTypes::Protoss_Fleet_Beacon] = fleetBeaconDesired;
	buildingDesired[UnitTypes::Protoss_Templar_Archives] = archivesDesired;
	buildingDesired[UnitTypes::Protoss_Observatory] = observatoryDesired;
	buildingDesired[UnitTypes::Protoss_Arbiter_Tribunal] = tribunalDesired;
}

void getBuildOrder()
{
	// Pylon, Forge, Nexus
	pylonDesired = min(22, (int)floor((Broodwar->self()->supplyUsed() / 14)));
	forgeDesired = min(1, ((int)floor(Broodwar->self()->supplyUsed() / 160)));
	nexusDesired = min(3, 1 + forceExpand + secondExpand + inactiveNexusCnt);
	gateNum = 2 + (Broodwar->self()->supplyUsed() / 60);

	// If we just attacked, expand
	if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Reaver) > 0 && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Shuttle) > 0)
	{
		forceExpand = 1;
	}

	// If our minerals exceed 800, expand
	if (Broodwar->self()->minerals() > 500)
	{
		secondExpand = 1;
	}

	// If we are teching up, ignore opener and rush adaptations
	if (Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Cybernetics_Core) > 0)
	{
		fourPool = false;
		twoGate = false;
		twoRax = false;
		getOpener = false;
	}

	// If not opener, gas is based on whether we need more or not
	if (!getOpener)
	{
		if (Broodwar->self()->gas() * 2 < Broodwar->self()->minerals())
		{
			gasDesired = min(Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Gateway), (int)geysers.size());
		}
	}	

	// Robotics 
	if (Broodwar->self()->supplyUsed() >= 160)
	{
		roboDesired = min(2, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus));
	}
	else if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Dragoon) > 4)
	{
		roboDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus));
	}



	switch (Broodwar->enemy()->getRace())
	{
	case Races::Enum::Zerg:
		// Currently use the same opener regardless of four pool or not
		if (fourPool || getOpener)
		{
			myOpeners(0);
			currentStrategy.assign("Two Gate Core");
		}
		else
		{
			currentStrategy.assign("Three Gate Reaver");
			myBuilds(0);
		}
		break;
	case Races::Enum::Terran:
		if (twoRax && getOpener)
		{
			myOpeners(0);
			currentStrategy.assign("Two Gate Core");
		}
		else if (getOpener)
		{
			myOpeners(1);
			currentStrategy.assign("One Gate Core");
		}
		else
		{
			myBuilds(1);
			currentStrategy.assign("Two Base Arbiter");
		}
		break;
	case Races::Enum::Protoss:
		if (twoGate && getOpener)
		{
			myOpeners(0); // 2 gate
			currentStrategy.assign("Two Gate Core");
		}
		else if (getOpener)
		{
			myOpeners(1); // 1 gate 
			currentStrategy.assign("One Gate Core");
		}
		else
		{
			myBuilds(0);
			currentStrategy.assign("Three Gate Reaver");
		}
		break;
	case Races::Enum::Random:
	{
		myOpeners(0); // 2 Gate opener
		currentStrategy.assign("Two Gate Core");
	}
	break;
	case Races::Enum::Unknown:
	{
		myOpeners(0); // 2 Gate opener
		currentStrategy.assign("Two Gate Core");
	}
	break;
	}


	// Annoying mapping process
	myBuildings();
}

void myBuilds(int whichBuild)
{
	switch (whichBuild){
		// -- Robo and High Templar Lategame --
	case 0:
		// First Base Tech - Reavers		
		supportBayDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Robotics_Facility));
		observatoryDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Robotics_Facility));
		gateDesired = min(1 + 2*Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus), gateNum);

		// Second Base Tech - Speedlots and High Templars
		citadelDesired = min(1, max(0, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) + Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Robotics_Facility) - 2));
		archivesDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Citadel_of_Adun));
		break;
		// -- Robo and Arbiter Lategame	--	
	case 1:
		// First Base Tech - Reavers
		supportBayDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Robotics_Facility));
		observatoryDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Robotics_Facility));
		gateDesired = min(1 + 2 * Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus), gateNum);

		// Second Base Tech	- Speedlots and Arbiters		
		stargateDesired = min(1, Broodwar->self()->supplyUsed()/160);
		citadelDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Stargate));
		archivesDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Citadel_of_Adun));
		tribunalDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Templar_Archives));
		break;
	}
}

void myOpeners(int whichOpener)
{
	switch (whichOpener)
	{
	case 0:
		// 2 Gate Core		
		coreDesired = min(1, (int)floor(Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Zealot) / 4));
		gasDesired = min((int)geysers.size(), (int)floor(Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Zealot) / 4));
		if (Broodwar->self()->supplyUsed() >= 20 && Broodwar->self()->supplyUsed() < 24)
		{
			gateDesired = 1;
		}
		else if (Broodwar->self()->supplyUsed() >= 24)
		{
			gateDesired = 2;
		}
		break;
	case 1:
		// 1 Gate Core		
		coreDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Gateway));
		if (Broodwar->self()->supplyUsed() >= 20)
		{
			gateDesired = 1;
		}
		if (Broodwar->self()->supplyUsed() >= 24)
		{
			gasDesired = 1;
		}
		break;
	}
}

