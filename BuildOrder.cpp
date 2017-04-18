#include "BuildOrder.h"

using namespace BWAPI;
bool getEarlyBuild = true, getMidBuild = false, getLateBuild = false;

// Building consistency order: nexus, pylon, gas, gate, forge, core, robo, stargate, citadel, support, fleet, archives, observatory, tribunal
// Changes: Terran only runs 20 Nexus!
// Make building desired into a class for easier storage

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

void desiredBuildings()
{	
	// Pylon, Forge, Nexus
	pylonDesired = min(22, (int)floor((supply / max(12, (16 - Broodwar->self()->allUnitCount(UnitTypes::Protoss_Pylon))))));
	forgeDesired = min(1,Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus)/3);
	nexusDesired = Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Nexus);

	// If we are saturated, expand
	if (saturated && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Gateway) >= (2 + Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Nexus) - inactiveNexusCnt) && idleGates.size() == 0)
	{
		nexusDesired++;
	}		
	
	// If forcing an early natural expansion
	if (forceExpand == 1 && Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Nexus) == 1)
	{
		nexusDesired++;
	}

	// If no idle gates and we are floating minerals, add 1 more
	if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Gateway) >= 1 && idleGates.size() == 0 && Broodwar->self()->minerals() > 300 && nexusDesired == Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Nexus))
	{
		gateDesired = min(Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) * 3, Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Gateway) + 1);
	}

	// If we have stabilized and have 4 dragoons, time to tech to mid game, ignore enemy early aggresion
	if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Cybernetics_Core) > 0 && idleGates.size() == 0)
	{		
		getEarlyBuild = false;
		getMidBuild = true;
		noZealots = false;
	}

	if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Cybernetics_Core) > 0)
	{
		enemyAggresion = false;
	}

	// If we are in mid game builds and we hit at least 4 gates, chances are we need to tech again
	if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Gateway) >= 4 && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) > 2 && getMidBuild)
	{		
		getMidBuild = false;
		getLateBuild = true;
	}

	// If not early build, gas is now based on whether we need more or not rather than a supply amount	
	if (!getEarlyBuild && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) == Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Nexus) && Broodwar->self()->gas() < 50)
	{
		gasDesired = geysers.size();
	}
	
}

void getBuildOrder()
{
	desiredBuildings();
	{
		switch (Broodwar->enemy()->getRace())
		{
			/* Protoss vs Zerg		Early Game: 2 Gate Core		Mid Game Tech: Speedlots		Late Game Tech: High Temps and Dark Archons	*/
			// IMPLEMENTING -- If Muta, mid build 2 (corsairs)
		case Races::Enum::Zerg:			
			if (enemyAggresion || getEarlyBuild)
			{
				earlyBuilds(0);				
			}
			else if (getMidBuild)
			{
				midBuilds(0);				
			}
			else if (getLateBuild)
			{
				lateBuilds(1);
			}
			break;

			/* Protoss vs Terran		Early Game: 1 Gate Core		Mid Game Tech: Reavers		Late Game Tech: High Temps and Arbiters	*/
		case Races::Enum::Terran:
			if (enemyAggresion && getEarlyBuild)
			{
				earlyBuilds(0);				
			}
			else if (getEarlyBuild)
			{
				earlyBuilds(1);				
			}
			else if (getMidBuild)
			{
				if (terranBio)
				{
					midBuilds(3);
				}
				else if (forceExpand)
				{
					midBuilds(3);
				}
				else
				{
					midBuilds(3);
				}
			}
			else if (getLateBuild)
			{
				lateBuilds(0);
			}
			break;

			/* Protoss vs Protoss		Early Game: 2 Gate Core		Mid Game Tech: Reavers		Late Game Tech: High Temps */
		case Races::Enum::Protoss:
			if (enemyAggresion || getEarlyBuild)
			{
				earlyBuilds(0); 	
			}
			else if (getMidBuild)
			{
				midBuilds(0);
				
			}
			else if (getLateBuild)
			{
				lateBuilds(1);
			}
			break;
		case Races::Enum::Random:
		{
			earlyBuilds(0);
		}
		break;
		case Races::Enum::Unknown:
		{
			earlyBuilds(0);
		}
		break;
		}
	}
	// Annoying mapping process	
	myBuildings();
}

void midBuilds(int whichBuild)
{
	switch (whichBuild){
	case 0:
		// -- Reavers --		
		roboDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus));
		supportBayDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Robotics_Facility));
		observatoryDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Robotics_Facility));
		currentStrategy.assign("Robo Tech");
		break;

	case 1:
		// -- Speedlots	--	
		citadelDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus));		
		currentStrategy.assign("Speedlot Tech");
		break;

	case 2:
		// -- Corsairs --
		stargateDesired = min(2, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Dragoon) / 4);
		currentStrategy.assign("Corsair Tech");
		break;
	case 3:
		// -- 2 Nexus Reaver --
		nexusDesired = max(2, nexusDesired);
		if (Broodwar->self()->supplyUsed() >= 60)
		{
			roboDesired = min(1, Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Nexus) / 2);
		}
		supportBayDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Robotics_Facility));
		currentStrategy.assign("Range Robo Expand");
	}
}

void lateBuilds(int whichBuild)
{
	switch (whichBuild)
	{
	case 0:
		// -- Arbiters and High Templars -- 
		citadelDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus));
		archivesDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Citadel_of_Adun));
		stargateDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Templar_Archives));
		tribunalDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Templar_Archives));
		currentStrategy.assign("Arbiter and Templar Tech");
		break;
	case 1:
		// -- High Templars and Dark Templars --
		citadelDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus));
		archivesDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Citadel_of_Adun));
		currentStrategy.assign("Templar Tech");
		break;
	case 2:
		// -- Carriers --
		stargateDesired = min(2, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) + Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Fleet_Beacon));
		fleetBeaconDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Stargate));
		currentStrategy.assign("Carrier Tech");
		break;
	}

}

void earlyBuilds(int whichBuild)
{
	switch (whichBuild)
	{
	case 0:
		// -- 2 Gate Core --
		coreDesired = min(1, (int)floor(Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Zealot) / 4));
		gasDesired = min(1, (int)floor(Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Zealot) / 2));
		if (supply >= 20 && supply < 24)
		{
			gateDesired = 1;
		}
		else if (supply >= 24)
		{
			gateDesired = 2;
		}
		currentStrategy.assign("Two Gate Core");
		break;
	case 1:
		// -- 1 Gate Core --	
		noZealots = true;
		coreDesired = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Gateway));
		if (supply >= 20)
		{
			gateDesired = 1;
		}
		if (supply >= 22)
		{
			gasDesired = 1;
		}
		if (supply >= 26)
		{
			coreDesired = 1;
		}
		if (supply >= 36)
		{
			gateDesired = 2;
		}
		currentStrategy.assign("One Gate Core");
		break;
	case 2:
		// -- 12 Nexus --
		if (supply >= 24)
		{
			nexusDesired = 2;
		}
		currentStrategy.assign("Early Expand");
		break;
	}
}

// PvT range expand:
//8 pylon, 10 gateway, 12 gas, 13 cyber, 15 pylon, 17 dragoon range, 18 gateway, 20 nexus, 20 2 dragoons, 24 pylon, 25 2 dragoons, 31 robo

//  https://pastebin.com/Kq0GDyfi