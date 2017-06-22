#include "McRave.h"

void BuildOrderTrackerClass::update()
{
	updateBuildStage();
	updateBaseBuild();
	updateSituationalBuild();
}

void BuildOrderTrackerClass::updateBuildStage()
{
	if (Broodwar->self()->getRace() == Races::Protoss)
	{
		// Done opening book
		if (buildingDesired[UnitTypes::Protoss_Cybernetics_Core] >= 1 && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Gateway) >= 2 && getEarlyBuild)
		{
			getEarlyBuild = false;
		}

		// If we have stabilized and have 4 dragoons, time to tech to mid game
		if (!getEarlyBuild && (Strategy().isRush() && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Dragoon) >= 6) || (!Strategy().isRush() && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Dragoon) >= 2 && Production().getIdleGates().size() == 0))
		{
			getMidBuild = true;
		}

		if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Reaver) >= 1 && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Observer) >= 1 && getMidBuild)
		{
			getMidBuild = false;
		}

		// If we are in mid game builds and we hit at least 4 gates, chances are we need to tech again
		if (!getMidBuild && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Gateway) >= 4 && Production().getIdleGates().size() == 0)
		{
			getLateBuild = true;
		}
	}
	else if (Broodwar->self()->getRace() == Races::Terran)
	{
		if (buildingDesired[UnitTypes::Terran_Factory] >= 1)
		{
			getEarlyBuild = false;
		}
	}
	return;
}

void BuildOrderTrackerClass::updateBaseBuild()
{
	if (Broodwar->self()->getRace() == Races::Protoss)
	{
		// Pylon, Forge, Nexus
		buildingDesired[UnitTypes::Protoss_Pylon] = min(22, (int)floor((Units().getSupply() / max(14, (16 - Broodwar->self()->allUnitCount(UnitTypes::Protoss_Pylon))))));
		buildingDesired[UnitTypes::Protoss_Forge] = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) / 3);		

		if (Strategy().getNumberZerg() > 0)
		{
			earlyBuild = 0;
			midBuild = Strategy().needDetection();
			lateBuild = 1;
		}
		else if (Strategy().getNumberProtoss() > 0)
		{
			earlyBuild = 1;
			midBuild = Strategy().needDetection();
			lateBuild = 1;
		}
		else if (Strategy().getNumberTerran() > 0)
		{
			earlyBuild = 3;
			if (Terrain().isWalled())
			{
				midBuild = 2;
			}
			else
			{
				midBuild = 0;
			}
			lateBuild = 0;
		}
		else
		{
			earlyBuild = 0;
			midBuild = Strategy().needDetection();
			lateBuild = 1;
		}
	}
	else if (Broodwar->self()->getRace() == Races::Terran)
	{
		buildingDesired[UnitTypes::Terran_Supply_Depot] = min(22, (int)floor((Units().getSupply() / max(14, (18 - Broodwar->self()->allUnitCount(UnitTypes::Terran_Supply_Depot))))));
		buildingDesired[UnitTypes::Terran_Armory] = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Terran_Command_Center) / 2);
		earlyBuild = 0;
		midBuild = 0;
		lateBuild = 0;
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

void BuildOrderTrackerClass::updateSituationalBuild()
{
	// Expansion logic
	if (!Strategy().isRush() && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) == Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Nexus) && ((Broodwar->self()->minerals() > 300 && Resources().isMinSaturated() && Production().isGateSat() && Production().getIdleGates().size() == 0) || (Strategy().isFastExpand() && Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Nexus) == 1)))
	{
		buildingDesired[UnitTypes::Protoss_Nexus] = Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Nexus) + 1;
	}

	// Shield battery logic
	if (Strategy().isRush())
	{
		buildingDesired[UnitTypes::Protoss_Shield_Battery] = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Gateway));
	}

	// Gateway logic
	if ((Broodwar->self()->minerals() - Production().getReservedMineral() - Buildings().getQueuedMineral() > 200) || (!Production().isGateSat() && Resources().isMinSaturated()))
	{
		buildingDesired[UnitTypes::Protoss_Gateway] = min(Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) * 3, Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Gateway) + 1);
	}

	// Assimilator logic
	if (Resources().isMinSaturated())
	{
		buildingDesired[UnitTypes::Protoss_Assimilator] = Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus);
	}
}

void BuildOrderTrackerClass::earlyBuilds()
{
	if (Broodwar->self()->getRace() == Races::Protoss)
	{
		// Two gate core
		if (earlyBuild == 0)
		{
			buildingDesired[UnitTypes::Protoss_Gateway] = (Units().getSupply() >= 20) + (Units().getSupply() >= 24);
			buildingDesired[UnitTypes::Protoss_Assimilator] = Units().getSupply() >= 48;
			buildingDesired[UnitTypes::Protoss_Cybernetics_Core] = Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Zealot) >= 4;
		}
		// One gate core - 2 Zealot
		else if (earlyBuild == 1)
		{
			buildingDesired[UnitTypes::Protoss_Gateway] = (Units().getSupply() >= 20) + (Units().getSupply() >= 38);
			buildingDesired[UnitTypes::Protoss_Assimilator] = Units().getSupply() >= 24;
			buildingDesired[UnitTypes::Protoss_Cybernetics_Core] = Units().getSupply() >= 36;
		}
		// One gate core - no Zealot
		else if (earlyBuild == 2)
		{
			buildingDesired[UnitTypes::Protoss_Gateway] = (Units().getSupply() >= 20) + (Units().getSupply() >= 36);
			buildingDesired[UnitTypes::Protoss_Assimilator] = Units().getSupply() >= 24;
			buildingDesired[UnitTypes::Protoss_Cybernetics_Core] = Units().getSupply() >= 26;
		}
		// 12 Nexus
		else if (earlyBuild == 3)
		{
			buildingDesired[UnitTypes::Protoss_Nexus] = 2 * (Units().getSupply() >= 24);
			buildingDesired[UnitTypes::Protoss_Gateway] = (Units().getSupply() >= 26) + (Units().getSupply() >= 32);
			buildingDesired[UnitTypes::Protoss_Assimilator] = Units().getSupply() >= 26;
			buildingDesired[UnitTypes::Protoss_Cybernetics_Core] = Units().getSupply() >= 30;
		}
	}
	else if (Broodwar->self()->getRace() == Races::Terran)
	{
		buildingDesired[UnitTypes::Terran_Barracks] = (Units().getSupply() >= 20);
	}
	return;
}

void BuildOrderTrackerClass::midBuilds()
{
	if (midBuild == 0)
	{	// Robo tech - Reavers first	
		buildingDesired[UnitTypes::Protoss_Robotics_Facility] = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Dragoon) / 2);
		buildingDesired[UnitTypes::Protoss_Robotics_Support_Bay] = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Robotics_Facility));
		buildingDesired[UnitTypes::Protoss_Observatory] = min(1, buildingDesired[UnitTypes::Protoss_Observatory] + Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Reaver));
	}
	if (midBuild == 1)
	{
		// Robo tech - Observers First
		buildingDesired[UnitTypes::Protoss_Robotics_Facility] = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Dragoon) / 2);
		buildingDesired[UnitTypes::Protoss_Observatory] = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Robotics_Facility));
		buildingDesired[UnitTypes::Protoss_Robotics_Support_Bay] = min(1, Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Observer));
	}
	if (midBuild == 2)
	{
		// Robo tech - Nexus first
		buildingDesired[UnitTypes::Protoss_Nexus] = max(2, buildingDesired[UnitTypes::Protoss_Nexus]);
		if (Broodwar->self()->supplyUsed() >= 60)
		{
			buildingDesired[UnitTypes::Protoss_Robotics_Facility] = min(1, Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Nexus) / 2);
		}
		buildingDesired[UnitTypes::Protoss_Robotics_Support_Bay] = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Robotics_Facility));
		buildingDesired[UnitTypes::Protoss_Observatory] = min(1, buildingDesired[UnitTypes::Protoss_Observatory] + Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Reaver));
	}
	if (midBuild == 3)
	{
		// Corsairs
		buildingDesired[UnitTypes::Protoss_Stargate] = min(2, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Dragoon) / 4);
	}
	if (midBuild == 4)
	{
		// Templar tech
		buildingDesired[UnitTypes::Protoss_Citadel_of_Adun] = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus));
		buildingDesired[UnitTypes::Protoss_Templar_Archives] = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Templar_Archives));
	}
	return;
}

void BuildOrderTrackerClass::lateBuilds()
{
	if (lateBuild == 0)
	{
		// Arbiter
		buildingDesired[UnitTypes::Protoss_Citadel_of_Adun] = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus));
		buildingDesired[UnitTypes::Protoss_Templar_Archives] = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Citadel_of_Adun));
		buildingDesired[UnitTypes::Protoss_Stargate] = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Templar_Archives));
		buildingDesired[UnitTypes::Protoss_Arbiter_Tribunal] = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Templar_Archives));
	}
	if (lateBuild == 1)
	{
		// Templar tech
		buildingDesired[UnitTypes::Protoss_Citadel_of_Adun] = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus));
		buildingDesired[UnitTypes::Protoss_Templar_Archives] = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Citadel_of_Adun));
	}
	if (lateBuild == 2)
	{
		// Carrier tech	
		buildingDesired[UnitTypes::Protoss_Stargate] = min(2, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) + Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Fleet_Beacon));
		buildingDesired[UnitTypes::Protoss_Fleet_Beacon] = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Stargate));
	}
	return;
}