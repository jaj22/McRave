#include "BuildOrder.h"
#include "GridManager.h"
#include "UnitManager.h"
#include "ResourceManager.h"
#include "ProductionManager.h"
#include "StrategyManager.h"

void BuildOrderTrackerClass::update()
{
	// Temporary variables
	int supply = UnitTracker::Instance().getSupply();
	bool saturated = ResourceTracker::Instance().isSaturated();
	bool forceExpand = false;	
	int inactiveNexusCnt = 0;

	// Pylon, Forge, Nexus
	buildingDesired[UnitTypes::Protoss_Pylon] = min(22, (int)floor((supply / max(12, (16 - Broodwar->self()->allUnitCount(UnitTypes::Protoss_Pylon))))));
	buildingDesired[UnitTypes::Protoss_Forge] = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) / 3);
	buildingDesired[UnitTypes::Protoss_Nexus] = Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Nexus);

	// If we are saturated, expand
	if (!getEarlyBuild && Broodwar->self()->minerals() > 300 && saturated && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Gateway) >= (2 + Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Nexus) - inactiveNexusCnt) && ProductionTracker::Instance().getIdleGates().size() == 0)
	{
		buildingDesired[UnitTypes::Protoss_Nexus]++;
	}

	// If forcing an early natural expansion
	if (forceExpand == 1 && Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Nexus) == 1)
	{
		buildingDesired[UnitTypes::Protoss_Nexus]++;
	}

	// If no idle gates and we are floating minerals, add 1 more
	if (Broodwar->self()->minerals() > 300 && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Gateway) >= 1 && ProductionTracker::Instance().getIdleGates().size() == 0 && buildingDesired[UnitTypes::Protoss_Nexus] == Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Nexus))
	{
		buildingDesired[UnitTypes::Protoss_Gateway] = min(Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) * 3, Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Gateway) + 1);
	}

	// If we have stabilized and have 4 dragoons, time to tech to mid game, ignore enemy early aggresion
	if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Cybernetics_Core) > 0 && ProductionTracker::Instance().getIdleGates().size() && (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Gateway) >= 2 || forceExpand))
	{
		getEarlyBuild = false;
		getMidBuild = true;
	}

	// If we are in mid game builds and we hit at least 4 gates, chances are we need to tech again
	if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Gateway) >= 4 && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) > 2 && getMidBuild)
	{
		getMidBuild = false;
		getLateBuild = true;
	}

	// If not early build, gas is now based on whether we need more or not rather than a supply amount	
	if (!getEarlyBuild && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) == buildingDesired[UnitTypes::Protoss_Nexus])
	{
		buildingDesired[UnitTypes::Protoss_Assimilator] = Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus);
	}

	switch (Broodwar->enemy()->getRace())
	{
		/* Protoss vs Zerg		Early Game: 2 Gate Core		Mid Game Tech: Reavers		Late Game Tech: High Temps and Dark Archons	*/
		// IMPLEMENTING -- If Muta, mid build 2 (corsairs)
	case Races::Enum::Zerg:
		earlyBuild = 0;
		midBuild = 0;
		lateBuild = 1;		
		break;

		/* Protoss vs Terran		Early Game: 1 Gate Core		Mid Game Tech: Reavers		Late Game Tech: High Temps and Arbiters	*/
	case Races::Enum::Terran:
		earlyBuild = 1;
		midBuild = 0;
		lateBuild = 0;
		break;

		/* Protoss vs Protoss		Early Game: 2 Gate Core		Mid Game Tech: Reavers		Late Game Tech: High Temps */
	case Races::Enum::Protoss:
		earlyBuild = 0;
		midBuild = 0;
		lateBuild = 1;
		break;
	case Races::Enum::Random:
	{
		earlyBuild = 0;
	}
	break;
	case Races::Enum::Unknown:
	{
		earlyBuild = 0;
	}
	break;
	}

	if (getEarlyBuild)
	{
		earlyBuilds();
	}
	else if (getMidBuild)
	{
		midBuilds();
	}
	else if (getLateBuild)
	{
		lateBuilds();
	}
}

void BuildOrderTrackerClass::earlyBuilds()
{
	int supply = UnitTracker::Instance().getSupply();
	switch (earlyBuild)
	{
	case 0:
		// -- 2 Gate Core --
		buildingDesired[UnitTypes::Protoss_Cybernetics_Core] = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Zealot) / 3);
		buildingDesired[UnitTypes::Protoss_Assimilator] = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Zealot) / 2);
		if (supply >= 20 && supply < 24)
		{
			buildingDesired[UnitTypes::Protoss_Gateway] = 1;
		}
		else if (supply >= 24)
		{
			buildingDesired[UnitTypes::Protoss_Gateway] = 2;
		}
		//currentStrategy.assign("Two Gate Core");
		break;
	case 1:
		// -- 1 Gate Core --		
		buildingDesired[UnitTypes::Protoss_Cybernetics_Core] = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Gateway));
		if (supply >= 20)
		{
			buildingDesired[UnitTypes::Protoss_Gateway] = 1;
		}
		if (supply >= 22)
		{
			buildingDesired[UnitTypes::Protoss_Assimilator] = 1;
		}
		if (supply >= 26)
		{
			buildingDesired[UnitTypes::Protoss_Cybernetics_Core] = 1;
		}
		if (supply >= 36)
		{
			buildingDesired[UnitTypes::Protoss_Gateway] = 2;
		}
		//currentStrategy.assign("One Gate Core");
		break;
	case 2:
		// -- 12 Nexus --
		if (supply >= 24)
		{
			buildingDesired[UnitTypes::Protoss_Nexus] = 2;
		}
		//currentStrategy.assign("Early Expand");
		break;
	}
}

void BuildOrderTrackerClass::midBuilds()
{
	int supply = UnitTracker::Instance().getSupply();
	switch (midBuild){
	case 0:
		// -- Reavers --		
		buildingDesired[UnitTypes::Protoss_Robotics_Facility] = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus));
		buildingDesired[UnitTypes::Protoss_Robotics_Support_Bay] = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Robotics_Facility));
		buildingDesired[UnitTypes::Protoss_Observatory] = min(1, Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Reaver));
		//currentStrategy.assign("Robo Tech");
		break;

	case 1:
		// -- Speedlots	--	
		buildingDesired[UnitTypes::Protoss_Citadel_of_Adun] = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus));
		//currentStrategy.assign("Speedlot Tech");
		break;

	case 2:
		// -- Corsairs --
		buildingDesired[UnitTypes::Protoss_Stargate] = min(2, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Dragoon) / 4);
		//currentStrategy.assign("Corsair Tech");
		break;
	case 3:
		// -- 2 Nexus Reaver --
		buildingDesired[UnitTypes::Protoss_Nexus] = max(2, buildingDesired[UnitTypes::Protoss_Nexus]);
		if (Broodwar->self()->supplyUsed() >= 60)
		{
			buildingDesired[UnitTypes::Protoss_Robotics_Facility] = min(1, Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Nexus) / 2);
		}
		buildingDesired[UnitTypes::Protoss_Robotics_Support_Bay] = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Robotics_Facility));
		//currentStrategy.assign("Range Robo Expand");
	}
}

void BuildOrderTrackerClass::lateBuilds()
{
	int supply = UnitTracker::Instance().getSupply();
	switch (lateBuild)
	{
	case 0:
		// -- Arbiters and High Templars -- 
		buildingDesired[UnitTypes::Protoss_Citadel_of_Adun] = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus));
		buildingDesired[UnitTypes::Protoss_Templar_Archives] = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Citadel_of_Adun));
		buildingDesired[UnitTypes::Protoss_Stargate] = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Templar_Archives));
		buildingDesired[UnitTypes::Protoss_Arbiter_Tribunal] = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Templar_Archives));
		//currentStrategy.assign("Arbiter and Templar Tech");
		break;
	case 1:
		// -- High Templars and Dark Templars --
		buildingDesired[UnitTypes::Protoss_Citadel_of_Adun] = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus));
		buildingDesired[UnitTypes::Protoss_Templar_Archives] = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Citadel_of_Adun));
		//currentStrategy.assign("Templar Tech");
		break;
	case 2:
		// -- Carriers --
		buildingDesired[UnitTypes::Protoss_Stargate] = min(2, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) + Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Fleet_Beacon));
		buildingDesired[UnitTypes::Protoss_Fleet_Beacon] = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Stargate));
		//currentStrategy.assign("Carrier Tech");
		break;
	}

}

// PvT range expand:
//8 pylon, 10 gateway, 12 gas, 13 cyber, 15 pylon, 17 dragoon range, 18 gateway, 20 nexus, 20 2 dragoons, 24 pylon, 25 2 dragoons, 31 robo

//  https://pastebin.com/Kq0GDyfi