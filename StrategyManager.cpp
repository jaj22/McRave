#include "McRave.h"

void StrategyTrackerClass::update()
{
	clock_t myClock;
	double duration = 0.0;
	myClock = clock();

	updateAlly();
	updateEnemy();
	updateSituationalBehaviour();

	duration = 1000.0 * double(clock() - myClock) / (double)CLOCKS_PER_SEC;
	//Broodwar->drawTextScreen(200, 70, "Strategy Manager: %d ms", duration);
}

void StrategyTrackerClass::updateAlly()
{
	// Reset
	globalEnemyStrength = 0.0;
	globalAllyStrength = 0.0;
	allyDefense = 0.0;
	enemyDefense = 0.0;

	// Check through all alive units or units dead within 500 frames
	for (auto &u : Units().getMyUnits())
	{
		// If deadframe is 0, unit is alive still
		if (u.second.unit() && u.second.getDeadFrame() == 0)
		{
			// Strength based calculations ignore workers and buildings
			if (!u.second.getType().isWorker() && !u.second.getType().isBuilding())
			{
				// Add strength				
				globalAllyStrength += u.second.getStrength();

				// Set last command frame
				if (u.first->isAttackFrame())
				{
					u.second.setLastAttackFrame(Broodwar->getFrameCount());
				}
			}
			if (u.second.getType().isBuilding() && u.second.getGroundDamage() > 0 && u.second.unit()->isCompleted())
			{
				allyDefense += u.second.getStrength();
			}
		}
		else
		{
			globalAllyStrength += u.second.getMaxStrength() * 0.5 / (1.0 + 0.001*(double(Broodwar->getFrameCount()) - double(u.second.getDeadFrame())));
		}
	}
}

void StrategyTrackerClass::updateEnemy()
{
	// Reset	
	eZerg = 0;
	eProtoss = 0;
	eTerran = 0;

	// Store enemy races
	for (auto &player : Broodwar->enemies())
	{
		if (player->getRace() == Races::Zerg)
		{
			eZerg++;
		}
		else if (player->getRace() == Races::Protoss)
		{
			eProtoss++;
		}
		else if (player->getRace() == Races::Terran)
		{
			eTerran++;
		}
	}

	// Stored enemy units iterator
	for (auto &u : Units().getEnUnits())
	{
		// If nullptr, continue
		if (!u.first)
		{
			continue;
		}
		// If deadframe is 0, unit is alive still
		if (u.second.getDeadFrame() == 0)
		{
			if (Workers().isScouting())
			{
				if (eTerran > 0 && theMap.GetArea(u.second.getTilePosition()))
				{
					for (auto &choke : theMap.GetArea(u.second.getTilePosition())->ChokePoints())
					{
						if (Position(choke->Center()).getDistance(u.second.getPosition()) < 256)
						{
							walledOff = true;
						}
					}
				}
			}

			enemyComposition[u.second.getType()] += 1;

			// If tile is visible but unit is not, remove position
			if (!u.first->exists() && u.second.getPosition() != Positions::None && Broodwar->isVisible(TilePosition(u.second.getPosition())))
			{
				u.second.setPosition(Positions::None);
			}

			// Strength based calculations ignore workers and buildings
			if (!u.second.getType().isWorker() && !u.second.getType().isBuilding())
			{
				// Add strength				
				globalEnemyStrength += u.second.getStrength();
			}
			if (u.second.getType().isBuilding() && u.second.getGroundDamage() > 0 && u.second.unit()->isCompleted())
			{
				enemyDefense += u.second.getStrength();
			}
		}

		// If unit is dead
		else if (u.second.getDeadFrame() != 0)
		{
			// Add a portion of the strength to ally strength
			globalAllyStrength += u.second.getMaxStrength() * 1 / (1.0 + 0.001*(double(Broodwar->getFrameCount()) - double(u.second.getDeadFrame())));
		}
	}
}

void StrategyTrackerClass::updateSituationalBehaviour()
{
	// Reset unit score
	for (auto &unit : unitScore)
	{
		unit.second = 0;
	}		

	// Invisible unit detection
	if (enemyComposition[UnitTypes::Protoss_Dark_Templar] > 0 || enemyComposition[UnitTypes::Protoss_Citadel_of_Adun] > 0 || enemyComposition[UnitTypes::Protoss_Templar_Archives] > 0 || enemyComposition[UnitTypes::Terran_Wraith] > 0 || enemyComposition[UnitTypes::Terran_Ghost] > 0 || enemyComposition[UnitTypes::Zerg_Lurker] > 0)
	{
		invis = true;
	}

	// Specific behaviours
	if (Broodwar->self()->getRace() == Races::Protoss)
	{
		protossStrategy();
	}
	else if (Broodwar->self()->getRace() == Races::Terran)
	{
		terranStrategy();
	}
	else if (Broodwar->self()->getRace() == Races::Zerg)
	{
		zergStrategy();
	}

	// Enemy composition
	int offset = 0;
	for (auto &t : enemyComposition)
	{
		// For each type, add a score to production based on the unit count divided by our current unit count
		updateUnitScore(t.first, t.second);
		if (t.first != UnitTypes::None && t.second > 0.0)
		{
			Broodwar->drawTextScreen(500, 50 + offset, "%s : %d", t.first.toString().c_str(), t.second);
			offset = offset + 10;
		}
		t.second = 0;
	}
}

void StrategyTrackerClass::protossStrategy()
{
	// Ramp holding logic
	if (Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Nexus) < 2 && (Broodwar->enemy()->getRace() == Races::Zerg && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Zealot) >= 3) || (Broodwar->enemy()->getRace() == Races::Protoss && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Dragoon) >= 1))
	{
		holdRamp = true;
	}
	else
	{
		holdRamp = false;
	}

	// If it's early on and we're being rushed
	if (Broodwar->self()->getUpgradeLevel(UpgradeTypes::Singularity_Charge) == 0)
	{
		if (eProtoss > 0 && (enemyComposition[UnitTypes::Protoss_Gateway] >= 2 || enemyComposition[UnitTypes::Protoss_Gateway] == 0) && enemyComposition[UnitTypes::Protoss_Assimilator] == 0 && Terrain().getEnemyBasePositions().size() > 0)
		{
			rush = true;
		}

		// If we are being 4/5 pooled, make a shield battery
		else if (eZerg > 0 && enemyComposition[UnitTypes::Zerg_Zergling] >= 4 && enemyComposition[UnitTypes::Zerg_Drone] <= 6)
		{
			rush = true;
		}

		// If we are being BBS'd
		else if (eTerran > 0 && enemyComposition[UnitTypes::Terran_Barracks] == 0 && enemyComposition[UnitTypes::Terran_Command_Center] > 0)
		{
			rush = true;
		}
		else
		{
			rush = false;
		}
	}
	else
	{
		rush = false;
	}

	// Forge fast expand Logic
	if (eZerg > 0)
	{
		//fastExpand = true; -- removed FFE for now
	}

	// Fast expand logic
	if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Nexus) < 2)
	{
		if (enemyComposition[UnitTypes::Terran_Bunker] > 0 || enemyComposition[UnitTypes::Protoss_Photon_Cannon] >= 2)
		{
			fastExpand = true;
		}
		else
		{
			fastExpand = false;
		}
	}
}

void StrategyTrackerClass::terranStrategy()
{
	// Ramp holding logic
	if (Broodwar->self()->completedUnitCount(UnitTypes::Terran_Academy) < 1)
	{
		holdRamp = false;
	}
	else
	{
		holdRamp = true;
	}
}

void StrategyTrackerClass::zergStrategy()
{

}

void StrategyTrackerClass::updateUnitScore(UnitType unit, int count)
{
	switch (unit)
	{
		if (Broodwar->enemy()->getRace() == Races::Terran)
		{
	case UnitTypes::Enum::Terran_Marine:
		unitScore[UnitTypes::Protoss_Zealot] += max(0.0, log(1 + (0.25*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot))))));
		unitScore[UnitTypes::Protoss_Dragoon] += max(0.0, log(1 + (0.75*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon))))));
		break;
	case UnitTypes::Enum::Terran_Medic:
		unitScore[UnitTypes::Protoss_Zealot] += max(0.0, log(1 + (0.25*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot))))));
		unitScore[UnitTypes::Protoss_Dragoon] += max(0.0, log(1 + (0.75*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon))))));
		break;
	case UnitTypes::Enum::Terran_Firebat:
		unitScore[UnitTypes::Protoss_Zealot] += max(0.0, log(1 + (0.25*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot))))));
		unitScore[UnitTypes::Protoss_Dragoon] += max(0.0, log(1 + (0.75*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon))))));
		break;
	case UnitTypes::Enum::Terran_Vulture:
		unitScore[UnitTypes::Protoss_Zealot] += max(0.0, log(1 + (0.25*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot))))));
		unitScore[UnitTypes::Protoss_Dragoon] += max(0.0, log(1 + (0.75*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon))))));
		break;
	case UnitTypes::Enum::Terran_Goliath:
		unitScore[UnitTypes::Protoss_Zealot] += max(0.0, log(1 + (0.50*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot))))));
		unitScore[UnitTypes::Protoss_Dragoon] += max(0.0, log(1 + (0.50*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon))))));
		break;
	case UnitTypes::Enum::Terran_Wraith:
		unitScore[UnitTypes::Protoss_Dragoon] += max(0.0, log(1 + (0.50*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon))))));
		break;
	case UnitTypes::Enum::Terran_Science_Vessel:
		unitScore[UnitTypes::Protoss_Dragoon] += max(0.0, log(1 + (0.50*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon))))));
		break;
	case UnitTypes::Enum::Terran_Battlecruiser:
		unitScore[UnitTypes::Protoss_Dragoon] += max(0.0, log(1 + (1.00*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon))))));
		break;
	case UnitTypes::Enum::Terran_Siege_Tank_Siege_Mode:
		unitScore[UnitTypes::Protoss_Zealot] += max(0.0, log(1 + (0.85*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot))))));
		unitScore[UnitTypes::Protoss_Dragoon] += max(0.0, log(1 + (0.15*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon))))));
		break;
	case UnitTypes::Enum::Terran_Siege_Tank_Tank_Mode:
		unitScore[UnitTypes::Protoss_Zealot] += max(0.0, log(1 + (0.85*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot))))));
		unitScore[UnitTypes::Protoss_Dragoon] += max(0.0, log(1 + (0.15*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon))))));
		break;
		}
		if (Broodwar->enemy()->getRace() == Races::Zerg)
		{
	case UnitTypes::Enum::Zerg_Zergling:
		unitScore[UnitTypes::Protoss_Zealot] += max(0.0, log(1 + (0.50*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot))))));
		unitScore[UnitTypes::Protoss_Dragoon] += max(0.0, log(1 + (0.50*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon))))));
		break;
	case UnitTypes::Enum::Zerg_Hydralisk:
		unitScore[UnitTypes::Protoss_Zealot] += max(0.0, log(1 + (0.25*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot))))));
		unitScore[UnitTypes::Protoss_Dragoon] += max(0.0, log(1 + (0.75*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon))))));
		break;
	case UnitTypes::Enum::Zerg_Lurker:
		unitScore[UnitTypes::Protoss_Zealot] += max(0.0, log(1 + (0.25*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot))))));
		unitScore[UnitTypes::Protoss_Dragoon] += max(0.0, log(1 + (0.75*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon))))));
		break;
	case UnitTypes::Enum::Zerg_Ultralisk:
		unitScore[UnitTypes::Protoss_Zealot] += max(0.0, log(1 + (0.25*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot))))));
		unitScore[UnitTypes::Protoss_Dragoon] += max(0.0, log(1 + (0.75*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon))))));
		break;
	case UnitTypes::Enum::Zerg_Mutalisk:
		unitScore[UnitTypes::Protoss_Dragoon] += max(0.0, log(1 + (0.75*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon))))));
		break;
	case UnitTypes::Enum::Zerg_Guardian:
		unitScore[UnitTypes::Protoss_Dragoon] += max(0.0, log(1 + (0.75*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon))))));
		break;
	case UnitTypes::Enum::Zerg_Defiler:
		unitScore[UnitTypes::Protoss_Zealot] += max(0.0, log(1 + (0.75*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot))))));
		unitScore[UnitTypes::Protoss_Dragoon] += max(0.0, log(1 + (0.25*double(count) / (1.0 + double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon))))));
		break;
		}
		if (Broodwar->enemy()->getRace() == Races::Protoss)
		{

		}
	}
}