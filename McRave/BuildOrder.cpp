#include "McRave.h"

void BuildOrderTrackerClass::update()
{
	updateBuildDecision();
	updateBaseBuild();
	Display().performanceTest(__func__);
	return;
}

void BuildOrderTrackerClass::updateBuildDecision()
{
	if (Broodwar->self()->getRace() == Races::Protoss)
	{
		// Done opening book
		if (buildingDesired[UnitTypes::Protoss_Cybernetics_Core] >= 1 && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Gateway) >= 2 && getEarlyBuild)
		{
			getEarlyBuild = false;
		}

		// If we have stabilized and have 4 dragoons, time to tech to mid game
		if (!getEarlyBuild && (Strategy().isRush() && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Dragoon) >= 6) || (!Strategy().isRush() && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Dragoon) >= 2 && Production().getIdleLowProduction().size() == 0))
		{
			getMidBuild = true;
		}

		// If we have our robo tech choice, mid build is over
		if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Reaver) >= 1 && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Observer) >= 1 && getMidBuild)
		{
			getMidBuild = false;
		}

		// If we are in mid game builds and we hit at least 4 gates, chances are we need to tech again
		if (!getMidBuild && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Gateway) >= 4 && Production().getIdleLowProduction().size() == 0)
		{
			getLateBuild = true;
		}
	}
	else if (Broodwar->self()->getRace() == Races::Terran)
	{
		// Done opening book
		if (Broodwar->self()->completedUnitCount(UnitTypes::Terran_Academy) >= 1)
		{
			getEarlyBuild = false;
		}

		if (Broodwar->self()->completedUnitCount(UnitTypes::Terran_Command_Center) >= 2)
		{
			getMidBuild = true;
		}
	}
	return;
}

void BuildOrderTrackerClass::updateBaseBuild()
{
	// Protoss
	if (Broodwar->self()->getRace() == Races::Protoss)
	{
		// PvZ
		if (Strategy().getNumberZerg() > 0)
		{
			earlyBuild = 4;
			midBuild = 4;
			lateBuild = 1;
		}
		// PvP
		else if (Strategy().getNumberProtoss() > 0)
		{
			earlyBuild = 1;
			midBuild = Strategy().needDetection();
			lateBuild = 1;
		}
		// PvT
		else if (Strategy().getNumberTerran() > 0)
		{
			earlyBuild = 3;
			midBuild = 5;
			lateBuild = 0;
		}
		// PvR
		else
		{
			earlyBuild = 0;
			midBuild = Strategy().needDetection();
			lateBuild = 1;
		}

		// Check situational build
		protossSituational();
	}

	// Terran
	else if (Broodwar->self()->getRace() == Races::Terran)
	{
		// TvA
		buildingDesired[UnitTypes::Terran_Supply_Depot] = min(22, (int)floor((Units().getSupply() / max(14, (16 - Broodwar->self()->allUnitCount(UnitTypes::Terran_Supply_Depot))))));
		earlyBuild = 0;
		midBuild = 0;
		lateBuild = 0;

		// Check situational build
		terranSituational();
	}

	// Zerg
	else if (Broodwar->self()->getRace() == Races::Zerg)
	{
		
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

void BuildOrderTrackerClass::protossSituational()
{
	// Pylon logic
	if (Strategy().isFastExpand() && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Pylon) <= 0)
	{
		buildingDesired[UnitTypes::Protoss_Pylon] = Units().getSupply() >= 14;
	}
	else
	{
		buildingDesired[UnitTypes::Protoss_Pylon] = min(22, (int)floor((Units().getSupply() / max(14, (16 - Broodwar->self()->allUnitCount(UnitTypes::Protoss_Pylon))))));
	}

	// Expansion logic
	if (!Strategy().isRush() && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) == Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Nexus) && ((Broodwar->self()->minerals() > 300 && Resources().isMinSaturated() && Production().isGateSat() && Production().getIdleLowProduction().size() == 0) || (Strategy().isFastExpand() && Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Nexus) == 1)))
	{
		buildingDesired[UnitTypes::Protoss_Nexus] = Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Nexus) + 1;
	}

	// Shield battery logic
	if (Strategy().isRush())
	{
		buildingDesired[UnitTypes::Protoss_Shield_Battery] = min(1, Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Cybernetics_Core));
	}

	// Gateway logic
	if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Gateway) >= 2 && (Production().getIdleLowProduction().size() == 0 && ((Broodwar->self()->minerals() - Production().getReservedMineral() - Buildings().getQueuedMineral() > 200) || (!Production().isGateSat() && Resources().isMinSaturated()))))
	{
		buildingDesired[UnitTypes::Protoss_Gateway] = min(Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) * 3, Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Gateway) + 1);
	}

	// Assimilator logic
	if (Resources().isMinSaturated())
	{
		buildingDesired[UnitTypes::Protoss_Assimilator] = Resources().getMyGas().size();
	}

	// Forge logic
	if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) >= 3)
	{
		buildingDesired[UnitTypes::Protoss_Forge] = 1;
	}

	// Cannon logic
	if (!Strategy().isFastExpand() && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Forge))
	{
		buildingDesired[UnitTypes::Protoss_Photon_Cannon] = 0;
		for (auto &base : Bases().getMyBases())
		{
			if (Grids().getDefenseGrid(base.second.getTilePosition()) < Grids().getDistanceHome(base.second.getWalkPosition()) / 100)
			{
				buildingDesired[UnitTypes::Protoss_Photon_Cannon] += Grids().getDistanceHome(base.second.getWalkPosition()) / 100;
			}
		}
	}
	if (Strategy().isBust())
	{
		buildingDesired[UnitTypes::Protoss_Photon_Cannon] = 6;
	}
}

void BuildOrderTrackerClass::terranSituational()
{
	// Bunker logic
	if (Strategy().isRush())
	{
		buildingDesired[UnitTypes::Terran_Bunker] = 1;
	}

	// Refinery logic
	if (Resources().isMinSaturated())
	{
		buildingDesired[UnitTypes::Terran_Refinery] = Resources().getMyGas().size();
	}

	// Barracks logic
	if (Broodwar->self()->completedUnitCount(UnitTypes::Terran_Barracks) >= 3 && (Production().getIdleLowProduction().size() == 0 && ((Broodwar->self()->minerals() - Production().getReservedMineral() - Buildings().getQueuedMineral() > 200) || (!Production().isBarracksSat() && Resources().isMinSaturated()))))
	{
		buildingDesired[UnitTypes::Terran_Barracks] = min(Broodwar->self()->completedUnitCount(UnitTypes::Terran_Command_Center) * 3, Broodwar->self()->visibleUnitCount(UnitTypes::Terran_Barracks) + 1);
	}

	// CC logic
	if (Broodwar->self()->hasResearched(TechTypes::Stim_Packs))
	{
		buildingDesired[UnitTypes::Terran_Command_Center] = 2;
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
		// 14 Nexus
		else if (earlyBuild == 3)
		{
			buildingDesired[UnitTypes::Protoss_Nexus] = 2 * (Units().getSupply() >= 28);
			buildingDesired[UnitTypes::Protoss_Gateway] = (Units().getSupply() >= 20) + (Units().getSupply() >= 32);
			buildingDesired[UnitTypes::Protoss_Assimilator] = Units().getSupply() >= 30;
			buildingDesired[UnitTypes::Protoss_Cybernetics_Core] = Units().getSupply() >= 30;
		}
		// FFE
		else if (earlyBuild == 4)
		{
			buildingDesired[UnitTypes::Protoss_Forge] = Units().getSupply() >= 20;
			buildingDesired[UnitTypes::Protoss_Nexus] = 2 * (Units().getSupply() >= 28);
			buildingDesired[UnitTypes::Protoss_Photon_Cannon] = (Units().getSupply() >= 22) + (Units().getSupply() >= 24);
			buildingDesired[UnitTypes::Protoss_Gateway] = (Units().getSupply() >= 30) + (Units().getSupply() >= 46);
			buildingDesired[UnitTypes::Protoss_Assimilator] = Units().getSupply() >= 36;
			buildingDesired[UnitTypes::Protoss_Cybernetics_Core] = Units().getSupply() >= 42;
		}
	}
	else if (Broodwar->self()->getRace() == Races::Terran)
	{
		buildingDesired[UnitTypes::Terran_Barracks] = (Units().getSupply() >= 18) + (Units().getSupply() >= 20) + (Units().getSupply() >= 42);
		buildingDesired[UnitTypes::Terran_Engineering_Bay] = (Units().getSupply() >= 36);
		buildingDesired[UnitTypes::Terran_Refinery] = (Units().getSupply() >= 36);
		buildingDesired[UnitTypes::Terran_Academy] = (Units().getSupply() >= 48);
	}
	return;
}

void BuildOrderTrackerClass::midBuilds()
{
	if (Broodwar->self()->getRace() == Races::Protoss)
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
			// Corsair and DT
			buildingDesired[UnitTypes::Protoss_Stargate] = 1;
			buildingDesired[UnitTypes::Protoss_Citadel_of_Adun] = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Stargate));
			buildingDesired[UnitTypes::Protoss_Templar_Archives] = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Citadel_of_Adun));
		}
		if (midBuild == 4)
		{
			// Templar tech
			buildingDesired[UnitTypes::Protoss_Citadel_of_Adun] = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus));
			buildingDesired[UnitTypes::Protoss_Templar_Archives] = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Citadel_of_Adun));
		}
		if (midBuild == 5)
		{
			// Arbiter tech
			buildingDesired[UnitTypes::Protoss_Citadel_of_Adun] = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus));
			buildingDesired[UnitTypes::Protoss_Stargate] = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Citadel_of_Adun));
			buildingDesired[UnitTypes::Protoss_Templar_Archives] = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Citadel_of_Adun));
			buildingDesired[UnitTypes::Protoss_Arbiter_Tribunal] = min(1, Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Templar_Archives));
		}
	}
	else if (Broodwar->self()->getRace() == Races::Terran)
	{
		buildingDesired[UnitTypes::Terran_Factory] = 2 + Broodwar->self()->hasResearched(TechTypes::Tank_Siege_Mode);
	}
	return;
}

void BuildOrderTrackerClass::lateBuilds()
{
	if (Broodwar->self()->getRace() == Races::Protoss)
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
	}
	return;
}