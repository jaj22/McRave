#include "McRave.h"

void CommandTrackerClass::update()
{
	for (auto &u : Units().getMyUnits())
	{		
		// Special units have their own commands
		if (u.second.getType() == UnitTypes::Protoss_Observer || u.second.getType() == UnitTypes::Protoss_Arbiter || u.second.getType() == UnitTypes::Protoss_Shuttle)
		{
			continue;
		}
		getDecision(u.first, u.second.getTarget());
	}
}

void CommandTrackerClass::getDecision(Unit unit, Unit target)
{
	int offset = 0;
	double closestD = 0.0;
	Position closestP;

	// Stop offset required for units with animations
	if (unit->getType() == UnitTypes::Protoss_Dragoon)
	{
		offset = 9;
	}

	// Ignore the unit if it no longer exists, is locked down, maelstrommed, stassised, not powered or not completed
	if (!unit || !unit->exists() || unit->isLockedDown() || unit->isMaelstrommed() || unit->isStasised() || !unit->isCompleted())
	{
		return;
	}

	// If the unit is ready to perform an action after an attack (Dragoons require 9 frames after an attack)
	if (Broodwar->getFrameCount() - Units().getMyUnits()[unit].getLastAttackFrame() > offset - Broodwar->getLatencyFrames())
	{
		// Check if we can use a shield battery first
		if (Grids().getBatteryGrid(unit->getTilePosition().x, unit->getTilePosition().y) > 0 && ((unit->getLastCommand().getType() == UnitCommandTypes::Right_Click_Unit && unit->getShields() < 40) || unit->getShields() < 10))
		{
			if (unit->getLastCommand().getType() != UnitCommandTypes::Right_Click_Unit && unit->getClosestUnit(Filter::IsAlly && Filter::IsCompleted  && Filter::GetType == UnitTypes::Protoss_Shield_Battery && Filter::Energy > 10))
			{
				unit->rightClick(unit->getClosestUnit(Filter::IsAlly && Filter::GetType == UnitTypes::Protoss_Shield_Battery));
			}
			return;
		}

		// If globally behind
		if (Units().getGlobalStrategy() == 0 || Units().getGlobalStrategy() == 2)
		{
			// Check if we have a target
			if (target && target != nullptr)
			{
				// If locally ahead, fight
				if (Units().getMyUnits()[unit].getStrategy() == 1)
				{
					microTarget(unit, target);
					return;
				}
				// Else flee
				else if (Units().getMyUnits()[unit].getStrategy() == 0)
				{
					fleeTarget(unit, target);
					return;
				}
			}
			// Else defend
			defend(unit, target);
			return;
		}

		// If globally ahead
		else if (Units().getGlobalStrategy() == 1 && Terrain().getEnemyBasePositions().size() > 0)
		{
			// Check if we have a target
			if (target && target != nullptr)
			{
				// If locally behind, contain
				if (Units().getMyUnits()[unit].getStrategy() == 0 || Units().getMyUnits()[unit].getStrategy() == 2)
				{
					fleeTarget(unit, target);
					return;
				}
				// Else attack
				else if (target->exists())
				{
					microTarget(unit, target);
					return;
				}
				// Else attack move best location
				else
				{
					attackMove(unit, target);
					return;
				}
			}
		}
		// Else there must not be any enemy base positions, attack move to random positions
		attackMove(unit, target);
	}
	return;
}

void CommandTrackerClass::attackMove(Unit unit, Unit target)
{
	// If target doesn't exist, move towards it
	if (target && Units().getEnUnits()[target].getPosition().isValid())
	{
		if (unit->getOrderTargetPosition() != Units().getEnUnits()[target].getPosition() || unit->isStuck())
		{
			unit->move(Units().getEnUnits()[target].getPosition());
		}
		return;
	}

	// If no target, attack closest enemy expansion if there is any
	else if (Terrain().getEnemyBasePositions().size() > 0)
	{
		double closestD = 0.0;
		Position closestP;
		for (auto base : Terrain().getEnemyBasePositions())
		{
			if (unit->getDistance(base) < closestD || closestD == 0.0)
			{
				closestP = base;
				closestD = unit->getDistance(base);
			}
		}
		if (unit->getOrderTargetPosition() != closestP)
		{
			unit->move(closestP);
		}
	}
	else
	{
		if (!Probes().isScouting() && Terrain().getNextExpansion().size() > 0)
		{
			unit->attack(Position(Terrain().getNextExpansion().at(rand() % Terrain().getNextExpansion().size())));
		}
	}
	return;
}

void CommandTrackerClass::microTarget(Unit unit, Unit target)
{
	// Variables
	bool kite = false;
	int range = (int)Units().getMyUnits()[unit].getGroundRange();

	// High Templars are only using storms
	if (unit->getType() == UnitTypes::Protoss_High_Templar)
	{
		// If target is close enough to storm
		if (target && target->exists())
		{
			unit->useTech(TechTypes::Psionic_Storm, target);
			return;
		}
	}

	// If kiting unnecessary, disable
	if (target->getType().isBuilding() || unit->getType() == UnitTypes::Protoss_Corsair)
	{
		kite = false;
	}

	// Reavers should always kite away from their target if it has lower range
	else if (unit->getType() == UnitTypes::Protoss_Reaver && Units().getEnUnits()[target].getGroundRange() < range)
	{
		kite = true;
	}

	// If kiting is a good idea, enable
	else if (target->getType() == UnitTypes::Terran_Vulture_Spider_Mine || (range > 32 && unit->isUnderAttack()) || (Units().getEnUnits()[target].getGroundRange() <= range && (unit->getDistance(target) <= range - Units().getEnUnits()[target].getGroundRange() && Units().getEnUnits()[target].getGroundRange() > 0 && range > 32 || unit->getHitPoints() < 40)))
	{
		kite = true;
	}

	// If kite is true and weapon on cooldown, move
	if (kite && unit->getGroundWeaponCooldown() > 0)
	{
		fleeTarget(unit, target);
	}
	else if (unit->getGroundWeaponCooldown() <= 0)
	{
		// If unit receieved an attack command on the target already, don't give another order - TODO: Test if it could be removed maybe to prevent goon stop bug
		if (unit->getLastCommand().getType() == UnitCommandTypes::Attack_Unit && unit->getLastCommand().getTarget() == target)
		{
			return;
		}
		if (unit->getOrderTarget() != target || unit->isStuck())
		{
			unit->attack(target);
		}
		Units().getMyUnits()[unit].setTargetPosition(target->getPosition());
	}
	return;
}

void CommandTrackerClass::exploreArea(Unit unit)
{
	// Given a region, explore a random portion of it based on random metrics like:
	// Distance to enemy
	// Distance to home
	// Last explored
	// Unit deaths
	// Untouched resources
}

void CommandTrackerClass::fleeTarget(Unit unit, Unit target)
{
	// If either the unit or current target are invalid, return
	if (!unit || !target)
	{
		return;
	}

	WalkPosition start;
	if (unit->getType().isWorker())
	{
		start = Probes().getMyProbes()[unit].getMiniTile();
	}
	else
	{
		start = Units().getMyUnits()[unit].getMiniTile();
	}

	WalkPosition finalPosition = start;
	double highestMobility = 0.0;

	// Search a 16x16 (4 tiles) mini tile area around the unit for highest mobility	and lowest threat
	for (int x = start.x - 12; x <= start.x + 12 + (unit->getType().tileWidth() * 4); x++)
	{
		for (int y = start.y - 12; y <= start.y + 12 + (unit->getType().tileHeight() * 4); y++)
		{
			if (WalkPosition(x, y).isValid())
			{
				if (unit->getType() == UnitTypes::Protoss_Dragoon && Grids().getResourceGrid(x / 4, y / 4) > 0)
				{
					continue;
				}

				double mobility = double(Grids().getMobilityGrid(x, y));
				double threat = Grids().getEGroundGrid(x, y);
				double distance = Grids().getEGroundDistanceGrid(x, y);
				double distanceHome = double(pow(Grids().getDistanceHome(x, y), 0.1));				

				if (Grids().getAntiMobilityGrid(x, y) == 0 && (mobility / (1.0 + (distance * threat))) / distanceHome > highestMobility && (getRegion(TilePosition(x / 4, y / 4)) && getRegion(unit->getTilePosition()) && getRegion(TilePosition(x / 4, y / 4)) == getRegion(unit->getTilePosition()) || (getNearestChokepoint(TilePosition(x / 4, y / 4)) && Position(x * 8, y * 8).getDistance(getNearestChokepoint(TilePosition(x / 4, y / 4))->getCenter()) < 128)))
				{
					bool bestTile = true;
					for (int i = x - unit->getType().width() / 16; i < x + unit->getType().width() / 16; i++)
					{
						for (int j = y - unit->getType().height() / 16; j < y + unit->getType().height() / 16; j++)
						{
							if (WalkPosition(i, j).isValid())
							{
								// If mini tile exists on top of unit, ignore it
								if (i >= start.x && i < start.x + unit->getType().tileWidth() * 4 && j >= start.y && j < start.y + unit->getType().tileHeight() * 4)
								{
									continue;
								}
								if (Grids().getMobilityGrid(i, j) == 0 || Grids().getAntiMobilityGrid(i, j) == 1)
								{
									bestTile = false;
								}
							}
						}
					}
					if (bestTile)
					{
						highestMobility = (mobility / (1.0 + (distance * threat))) / distanceHome;
						finalPosition = WalkPosition(x, y);
					}
				}
			}
		}
	}
	if (finalPosition.isValid() && finalPosition != start)
	{
		Grids().updateAllyMovement(unit, finalPosition);
		Units().getMyUnits()[unit].setTargetPosition(Position(finalPosition));
		if (unit->getOrderTargetPosition() != Position(finalPosition) || unit->isStuck())
		{
			unit->move(Position(finalPosition));
		}
	}
	return;
}

void CommandTrackerClass::defend(Unit unit, Unit target)
{
	double closestD = 0.0;
	Position closestP;		

	// If expanding to natural, hold at natural choke
	if (Terrain().getEnemyBasePositions().size() > 0 && BuildOrder().getBuildingDesired()[UnitTypes::Protoss_Nexus] > Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Nexus))
	{
		closestP = Position(Terrain().getPath().at(1)->Center());
		if (Terrain().getAllyTerritory().find(getNearestChokepoint(TilePosition(Terrain().getPath().at(1)->Center()))->getRegions().second) != Terrain().getAllyTerritory().end() || Terrain().getAllyTerritory().find(getNearestChokepoint(TilePosition(Terrain().getPath().at(1)->Center()))->getRegions().first) != Terrain().getAllyTerritory().end())
		{
			closestP = Position(Terrain().getPath().at(2)->Center());
		}
		if (unit->getLastCommand().getTargetPosition() != closestP || unit->getLastCommand().getType() != UnitCommandTypes::Move || unit->isStuck())
		{
			unit->move(closestP);
		}
		return;
	}

	// Early on, defend mineral line
	if (Terrain().getEnemyBasePositions().size() == 0 || !Strategy().needZealotWall())
	{
		for (auto nexus : Nexuses().getMyNexus())
		{
			if (unit->getLastCommand().getTargetPosition() != (Position(nexus.second.getCannonPosition()) + Position(nexus.first->getPosition())) / 2 || unit->getLastCommand().getType() != UnitCommandTypes::Move || unit->isStuck())
			{
				unit->move((Position(nexus.second.getCannonPosition()) + Position(nexus.first->getPosition())) / 2);
			}
			return;
		}
	}

	// Defend chokepoint with concave
	int min = 128;
	int max = 256;
	closestD = 0.0;
	WalkPosition start = Units().getMyUnits()[unit].getMiniTile();
	WalkPosition bestPosition = start;
	if (unit->getType() == UnitTypes::Protoss_Zealot)
	{
		min = 64;
		max = 96;
	}
	for (auto choke : Terrain().getDefendHere())
	{
		closestD = unit->getPosition().getDistance(choke);
		for (int x = (choke.x / 8) - 25; x <= (choke.x / 8) + 25; x++)
		{
			for (int y = (choke.y / 8) - 25; y <= (choke.y / 8) + 25; y++)
			{
				if (WalkPosition(x, y).isValid() && Grids().getMobilityGrid(x, y) > 0 && Terrain().getAllyTerritory().find(getRegion(TilePosition(WalkPosition(x, y)))) != Terrain().getAllyTerritory().end() && Position(WalkPosition(x, y)).getDistance(choke) > min && Position(WalkPosition(x, y)).getDistance(choke) < max && Position(WalkPosition(x, y)).getDistance(choke) < closestD)
				{
					bool safeTile = true;
					for (int i = x - unit->getType().width() / 16; i < x + unit->getType().tileWidth() / 16; i++)
					{
						for (int j = y - unit->getType().height() / 8; j < y + unit->getType().tileHeight() / 16; j++)
						{
							// If mini tile exists on top of unit, ignore it
							if (i >= start.x && i < start.x + unit->getType().tileWidth() * 4 && j >= start.y && j < start.y + unit->getType().tileHeight() * 4)
							{
								//Broodwar->drawBoxMap(Position(i * 8, j * 8), Position(i * 8 + 8, j * 8 + 8), Broodwar->self()->getColor());
								continue;
							}
							else if (Grids().getAntiMobilityGrid(i, j) > 0 || Grids().getMobilityGrid(i, j) == 0)
							{
								safeTile = false;
							}
						}
					}
					if (safeTile)
					{
						bestPosition = WalkPosition(x, y);
						closestD = Position(WalkPosition(x, y)).getDistance(choke);
					}
				}
			}
		}
		if (bestPosition.isValid() && bestPosition != start)
		{
			if (unit->getLastCommand().getTargetPosition() != Position(bestPosition) || unit->getLastCommand().getType() != UnitCommandTypes::Move || unit->isStuck())
			{
				unit->move(Position(bestPosition));
				Grids().updateAllyMovement(unit, bestPosition);
				Units().getMyUnits()[unit].setTargetPosition(Position(bestPosition));
			}
			return;
		}
	}
	return;
}