#include "CommandManager.h"
#include "GridManager.h"
#include "StrategyManager.h"
#include "TerrainManager.h"
#include "UnitManager.h"
#include "NexusManager.h"

void CommandTrackerClass::update()
{
	for (auto &u : UnitTracker::Instance().getMyUnits())
	{
		updateLocalStrategy(u.first, u.second.getTarget());
		updateGlobalStrategy(u.first, u.second.getTarget());

		// For now, special units still relies on unit manager information somewhat but not for commands
		if (u.second.getUnitType() == UnitTypes::Protoss_High_Templar || u.second.getUnitType() == UnitTypes::Protoss_Arbiter || u.second.getUnitType() == UnitTypes::Protoss_Observer)
		{
			continue;
		}

		//// Latency returning for now, else make a decision
		//if (Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0)
		//{
		//	return;
		//}
		updateDecisions(u.first, u.second.getTarget());
	}
}

void CommandTrackerClass::updateDecisions(Unit unit, Unit target)
{
	double closestD = 0.0;
	Position closestP;

	// Ignore the unit if it no longer exists, is locked down, maelstrommed, stassised, not powered or not completed
	if (!unit || !unit->exists() || unit->isLockedDown() || unit->isMaelstrommed() || unit->isStasised() || !unit->isPowered() || !unit->isCompleted())
	{
		return;
	}

	// If Reaver, train scarabs
	if (unit->getType() == UnitTypes::Protoss_Reaver && unit->getScarabCount() < 5)
	{
		unit->train(UnitTypes::Protoss_Scarab);
	}

	// Check if we have a target
	if (target && target != nullptr)
	{
		// Retreat
		if (UnitTracker::Instance().getMyUnits()[unit].getStrategy() == 0)
		{
			// Create concave when containing units
			if (GridTracker::Instance().getEnemyGrd(unit->getTilePosition().x, unit->getTilePosition().y) == 0.0 && globalStrategy == 1)
			{
				Position fleePosition = unitFlee(unit, target);
				if (fleePosition != Positions::None)
				{
					UnitTracker::Instance().getMyUnits()[unit].setTargetPosition(fleePosition);
					unit->move(Position(fleePosition.x + rand() % 3 + (-1), fleePosition.y + rand() % 3 + (-1)));
				}
				return;
			}

			// For each defensive position, find closest one		
			for (auto position : TerrainTracker::Instance().getDefendHere())
			{
				if (unit->getDistance(position) < 320 || TerrainTracker::Instance().getAllyTerritory().find(getRegion(unit->getTilePosition())) != TerrainTracker::Instance().getAllyTerritory().end())
				{
					Position fleePosition = unitFlee(unit, target);
					if (fleePosition != Positions::None)
					{
						UnitTracker::Instance().getMyUnits()[unit].setTargetPosition(fleePosition);
						unit->move(Position(fleePosition.x + rand() % 3 + (-1), fleePosition.y + rand() % 3 + (-1)));
					}
					return;
				}
				if (unit->getDistance(position) <= closestD || closestD == 0.0)
				{
					closestD = unit->getDistance(position);
					closestP = position;
				}
			}

			// If last command was too far away from this position, move there
			if (unit->getLastCommand().getTargetPosition().getDistance(TerrainTracker::Instance().getDefendHere().at(0)) > 5)
			{
				unit->move(Position(TerrainTracker::Instance().getDefendHere().at(0).x + rand() % 3 + (-1), TerrainTracker::Instance().getDefendHere().at(0).y + rand() % 3 + (-1)));
			}
			return;
		}
		// Attack
		if (UnitTracker::Instance().getMyUnits()[unit].getStrategy() == 1 && target->exists())
		{
			unitMicroTarget(unit, target);
			return;
		}
	}

	if (globalStrategy == 0)
	{
		if (TerrainTracker::Instance().getEnemyBasePositions().size() > 0 && TerrainTracker::Instance().getAllyTerritory().size() > 0)
		{
			// Pick random enemy bases to attack (cap at ~3-4 units?)
			closestD = 1000.0;
			closestP = TerrainTracker::Instance().getDefendHere().at(0);

			// If we forced to expand, move to next choke to prevent blocking 
			if (StrategyTracker::Instance().isFastExpand())
			{
				closestP = Position(TerrainTracker::Instance().getPath().at(1)->Center());
				if (TerrainTracker::Instance().getAllyTerritory().find(getNearestChokepoint(TilePosition(TerrainTracker::Instance().getPath().at(1)->Center()))->getRegions().second) != TerrainTracker::Instance().getAllyTerritory().end() || TerrainTracker::Instance().getAllyTerritory().find(getNearestChokepoint(TilePosition(TerrainTracker::Instance().getPath().at(1)->Center()))->getRegions().first) != TerrainTracker::Instance().getAllyTerritory().end())
				{
					closestP = Position(TerrainTracker::Instance().getPath().at(2)->Center());
				}
			}
			else if (Broodwar->getFrameCount() < 6000)
			{
				for (auto nexus : NexusTracker::Instance().getMyNexus())
				{
					unit->move((Position(nexus.second.getStaticPosition()) + Position(nexus.first->getPosition())) / 2);
					return;
				}
			}
			else
			{
				// Check if we are close enough to any defensive position
				for (auto position : TerrainTracker::Instance().getDefendHere())
				{
					if (unit->getDistance(position) < 128)
					{
						closestP = position;
						break;
					}
					else if (unit->getDistance(position) <= closestD)
					{
						closestD = unit->getDistance(position);
						closestP = position;
					}
				}
			}
			if (unit->getLastCommand().getTargetPosition().getDistance(closestP) > 5 || unit->getLastCommandFrame() + 100 < Broodwar->getFrameCount())
			{
				unit->move(Position(closestP.x + rand() % 3 + (-1), closestP.y + rand() % 3 + (-1)));
				return;
			}
		}
		else
		{
			// Else just defend at nearest chokepoint (starting of game without scout information)
			unit->move((getNearestChokepoint(unit->getPosition()))->getCenter());
			return;
		}
	}

	// Check if we should attack
	if (globalStrategy == 1 && TerrainTracker::Instance().getEnemyBasePositions().size() > 0)
	{
		if (target)
		{
			unit->attack(UnitTracker::Instance().getEnUnits()[target].getPosition());
			return;
		}
		for (auto base : TerrainTracker::Instance().getEnemyBasePositions())
		{
			if (unit->getDistance(base) < closestD || closestD == 0.0)
			{
				closestP = base;
				closestD = unit->getDistance(base);
			}
		}
		unit->attack(closestP);
		return;
	}
}

void CommandTrackerClass::updateLocalStrategy(Unit unit, Unit target)
{
	// Variables for calculating local strengths	
	double enemyLocalStrength = 0.0, allyLocalStrength = 0.0, thisUnit = 0.0;
	Position targetPosition = UnitTracker::Instance().getEnUnits()[target].getPosition();
	int radius = min(800, 384 + UnitTracker::Instance().getSupply() * 4);

	int aLarge = UnitTracker::Instance().getMySizes()[UnitSizeTypes::Large];
	int aMedium = UnitTracker::Instance().getMySizes()[UnitSizeTypes::Medium];
	int aSmall = UnitTracker::Instance().getMySizes()[UnitSizeTypes::Small];

	int eLarge = UnitTracker::Instance().getEnSizes()[UnitSizeTypes::Large];
	int eMedium = UnitTracker::Instance().getEnSizes()[UnitSizeTypes::Medium];
	int eSmall = UnitTracker::Instance().getEnSizes()[UnitSizeTypes::Small];

	// Reset local
	UnitTracker::Instance().getMyUnits()[unit].setLocal(0);

	if (unit->getDistance(targetPosition) > 512)
	{
		UnitTracker::Instance().getMyUnits()[unit].setStrategy(3);
		return;
	}

	// Check every enemy unit being in range of the target
	for (auto &u : UnitTracker::Instance().getEnUnits())
	{
		// Reset unit strength
		thisUnit = 0.0;

		// Ignore workers, keep buildings (reinforcements and static defenses)
		if (u.second.getUnitType().isWorker() || (u.first && u.first->exists() && u.first->isStasised()))
		{
			continue;
		}

		// If a unit is within range of the target, add to local strength
		if (u.second.getPosition().getDistance(targetPosition) < radius)
		{
			if (aLarge > 0 || aMedium > 0 || aSmall > 0)
			{
				// If unit is cloaked or burrowed and not detected, drastically increase strength
				if ((u.first->isCloaked() || u.first->isBurrowed()) && !u.first->isDetected())
				{
					thisUnit = 20 * u.second.getStrength();
				}
				else if (u.first->getType().groundWeapon().damageType() == DamageTypes::Explosive)
				{
					thisUnit = u.second.getStrength() * ((aLarge*1.0) + (aMedium*0.75) + (aSmall*0.5)) / (aLarge + aMedium + aSmall);
				}
				else if (u.first->getType().groundWeapon().damageType() == DamageTypes::Concussive)
				{
					thisUnit = u.second.getStrength() * ((aLarge*1.0) + (aMedium*0.75) + (aSmall*0.5)) / (aLarge + aMedium + aSmall);
				}
				else
				{
					thisUnit = u.second.getStrength();
				}
			}
			else
			{
				thisUnit = u.second.getStrength();
			}
			// If enemy hasn't died, add to enemy. Otherwise, partially add to ally local
			if (u.second.getDeadFrame() == 0)
			{
				enemyLocalStrength += thisUnit;
			}
			else
			{
				allyLocalStrength += u.second.getMaxStrength() * 1.0 / (1.0 + 0.01*(double(Broodwar->getFrameCount()) - double(u.second.getDeadFrame())));
			}
		}
	}

	// Check every ally being in range of the target
	for (auto &u : UnitTracker::Instance().getMyUnits())
	{
		// Reset unit strength
		thisUnit = 0.0;

		// Ignore workers and buildings
		if (u.second.getUnitType().isWorker() || u.second.getUnitType().isBuilding())
		{
			continue;
		}

		// If a unit is within the range of the ally unit, add to local strength
		if (u.second.getPosition().getDistance(unit->getPosition()) < radius)
		{
			if (eLarge > 0 || eMedium > 0 || eSmall > 0)
			{
				// If shuttle, add units inside
				if (u.second.getUnitType() == UnitTypes::Protoss_Shuttle && u.first->getLoadedUnits().size() > 0)
				{
					// Assume reaver for damage type calculations
					for (Unit uL : u.first->getLoadedUnits())
					{
						thisUnit = UnitTracker::Instance().getMyUnits()[uL].getStrength();
					}
				}
				else
				{
					// Damage type calculations
					if (u.second.getUnitType().groundWeapon().damageType() == DamageTypes::Explosive)
					{
						thisUnit = u.second.getStrength() * ((eLarge*1.0) + (eMedium*0.75) + (eSmall*0.5)) / (eLarge + eMedium + eSmall);
					}
					else if (u.second.getUnitType().groundWeapon().damageType() == DamageTypes::Concussive)
					{
						thisUnit = u.second.getStrength() * ((eLarge*1.0) + (eMedium*0.75) + (eSmall*0.5)) / (eLarge + eMedium + eSmall);
					}
					else
					{
						thisUnit = u.second.getStrength();
					}
				}
			}
			else
			{
				thisUnit = u.second.getStrength();
			}

			// If ally hasn't died, add to ally. Otherwise, partially add to enemy local
			if (u.second.getDeadFrame() == 0)
			{
				allyLocalStrength += thisUnit;
			}
			else
			{
				enemyLocalStrength += u.second.getMaxStrength() * 1.0 / (1.0 + 0.01*(double(Broodwar->getFrameCount()) - double(u.second.getDeadFrame())));
			}
		}
	}

	// Store the difference of strengths 
	UnitTracker::Instance().getMyUnits()[unit].setLocal(allyLocalStrength - enemyLocalStrength);

	if (Broodwar->enemy()->getRace() != Races::Terran && Broodwar->getFrameCount() < 6000)
	{
		if (target && target->exists())
		{
			if (GridTracker::Instance().getResourceGrid(unit->getTilePosition().x, unit->getTilePosition().y) > 0)
			{
				UnitTracker::Instance().getMyUnits()[unit].setStrategy(1);
				return;
			}
			else
			{
				UnitTracker::Instance().getMyUnits()[unit].setStrategy(0);
				return;
			}
		}
	}

	// If we are in ally territory and have a target, force to fight	
	if (target && target->exists())
	{
		if (Broodwar->getFrameCount() > 6000 && TerrainTracker::Instance().getAllyTerritory().find(getRegion(target->getPosition())) != TerrainTracker::Instance().getAllyTerritory().end())
		{
			UnitTracker::Instance().getMyUnits()[unit].setStrategy(1);
			return;
		}		
	}

	// Force Zealots to engage on ramp
	if (TerrainTracker::Instance().getAllyTerritory().size() <= 1 && unit->getDistance(TerrainTracker::Instance().getDefendHere().at(0)) < 64 && unit->getType() == UnitTypes::Protoss_Zealot && unit->getUnitsInRadius(64, Filter::IsEnemy).size() > 0)
	{
		UnitTracker::Instance().getMyUnits()[unit].setStrategy(1);
		return;
	}

	// Force Zealots to stay on Tanks
	if (unit->getType() == UnitTypes::Protoss_Zealot && target->exists() && (UnitTracker::Instance().getEnUnits()[target].getUnitType() == UnitTypes::Terran_Siege_Tank_Siege_Mode || UnitTracker::Instance().getEnUnits()[target].getUnitType() == UnitTypes::Terran_Siege_Tank_Tank_Mode) && unit->getDistance(targetPosition) < 128)
	{
		UnitTracker::Instance().getMyUnits()[unit].setStrategy(1);
		return;
	}

	// If unit is in range of a target and not currently threatened, attack instead of running
	if (unit->getDistance(targetPosition) + unit->getType().width() / 2 + target->getType().width() / 2 <= UnitTracker::Instance().getMyUnits()[unit].getRange())
	{
		bool safeTile = true;
		for (auto miniTile : UnitTracker::Instance().getMiniTilesUnderUnit(unit))
		{
			if (GridTracker::Instance().getEnemyMiniGrd(miniTile.x, miniTile.y) > 0.0)
			{
				safeTile = false;
			}
		}
		if (safeTile || unit->getType() == UnitTypes::Protoss_Reaver)
		{
			UnitTracker::Instance().getMyUnits()[unit].setStrategy(1);
			return;
		}
	}

	// If last command was engage
	if (UnitTracker::Instance().getMyUnits()[unit].getStrategy() == 1)
	{
		// Latch based system for at least 80% disadvantage to disengage
		if (allyLocalStrength < enemyLocalStrength*0.8)
		{
			UnitTracker::Instance().getMyUnits()[unit].setStrategy(0);
			return;
		}
		UnitTracker::Instance().getMyUnits()[unit].setStrategy(1);
		return;
	}
	// If last command was disengage/no command
	else
	{
		// Latch based system for at least 120% advantage to engage
		if (allyLocalStrength >= enemyLocalStrength*1.2)
		{
			UnitTracker::Instance().getMyUnits()[unit].setStrategy(1);
			return;
		}
		// Otherwise return 3 or 0, whichever was previous
		UnitTracker::Instance().getMyUnits()[unit].setStrategy(0);
		return;
	}
	// Disregard local if no target, no recent local calculation and not within ally region
	UnitTracker::Instance().getMyUnits()[unit].setStrategy(3);
	return;
}

void CommandTrackerClass::updateGlobalStrategy(Unit unit, Unit target)
{
	if (StrategyTracker::Instance().isFastExpand())
	{
		globalStrategy = 0;
		return;
	}

	// If Zerg, wait for a larger army before moving out
	if (Broodwar->enemy()->getRace() != Races::Terran && Broodwar->getFrameCount() < 6000)
	{
		globalStrategy = 0;
		return;
	}

	if (StrategyTracker::Instance().globalAlly() > StrategyTracker::Instance().globalEnemy())
	{		
		globalStrategy = 1;
		return;
	}
	else
	{
		// If not Zerg, contain
		if (Broodwar->enemy()->getRace() != Races::Zerg)
		{
			globalStrategy = 1;
			return;
		}
		globalStrategy = 0;
		return;
	}
	globalStrategy = 1;
	return;
}

void CommandTrackerClass::unitMicroTarget(Unit unit, Unit target)
{
	// Variables
	bool kite = false;
	int range = (int)UnitTracker::Instance().getMyUnits()[unit].getRange();
	int offset = 0;

	// Stop offset required for units with animations
	if (unit->getType() == UnitTypes::Protoss_Dragoon)
	{
		offset = 9;
	}

	// If kiting unnecessary, disable
	if (target->getType().isBuilding() || unit->getType() == UnitTypes::Protoss_Corsair)
	{
		kite = false;
	}

	// Reavers should always kite away from their target if it has lower range
	else if (unit->getType() == UnitTypes::Protoss_Reaver && UnitTracker::Instance().getEnUnits()[target].getRange() < range)
	{
		kite = true;
	}

	// If kiting is a good idea, enable
	else if (target->getType() == UnitTypes::Terran_Vulture_Spider_Mine || (range > 32 && unit->isUnderAttack()) || (UnitTracker::Instance().getEnUnits()[target].getRange() <= range && (unit->getDistance(target) <= range - UnitTracker::Instance().getEnUnits()[target].getRange() && UnitTracker::Instance().getEnUnits()[target].getRange() > 0 && range > 32 || unit->getHitPoints() < 40)))
	{
		kite = true;
	}

	// If kite is true and weapon on cooldown, move
	if (Broodwar->getFrameCount() - UnitTracker::Instance().getMyUnits()[unit].getLastCommandFrame() > offset - Broodwar->getLatencyFrames())
	{
		if (kite && unit->getGroundWeaponCooldown() > 0)
		{
			Position fleePosition = unitFlee(unit, target);
			UnitTracker::Instance().getMyUnits()[unit].setTargetPosition(fleePosition);
			if (fleePosition != BWAPI::Positions::None)
			{
				unit->move(Position(fleePosition.x + rand() % 3 + (-1), fleePosition.y + rand() % 3 + (-1)));
			}
		}
		else if (unit->getGroundWeaponCooldown() <= 0)
		{
			// If unit receieved an attack command on the target already, don't give another order - TODO: Test if it could be removed maybe to prevent goon stop bug
			if (unit->getLastCommand().getType() == UnitCommandTypes::Attack_Unit && unit->getLastCommand().getTarget() == target)
			{
				return;
			}
			unit->attack(target);
			UnitTracker::Instance().getMyUnits()[unit].setTargetPosition(target->getPosition());
		}
	}
	return;
}

void CommandTrackerClass::unitExploreArea(Unit unit)
{
	// Given a region, explore a random portion of it based on random metrics like:
	// Distance to enemy
	// Distance to home
	// Last explored
	// Unit deaths
	// Untouched resources
}

Position CommandTrackerClass::unitFlee(Unit unit, Unit target)
{
	// If either the unit or current target are invalid, return
	if (!unit || !target)
	{
		return Positions::None;
	}
	WalkPosition start = UnitTracker::Instance().getMyUnits()[unit].getMiniTile();
	Position currentTargetPosition = UnitTracker::Instance().getEnUnits()[unit].getTargetPosition();
	WalkPosition finalPosition = UnitTracker::Instance().getMyUnits()[unit].getMiniTile();
	double highestMobility = 0.0;
	double distanceTarget = unit->getDistance(target);
	double distanceHome = unit->getDistance(TerrainTracker::Instance().getPlayerStartingPosition());

	// Search a 30x30 (31) mini tile area around the unit for mobility	
	for (int x = start.x - 30; x <= start.x + 30; x++)
	{
		for (int y = start.y - 30; y <= start.y + 30; y++)
		{
			distanceTarget = Position(x * 8, y * 8).getDistance(currentTargetPosition);
			distanceHome = Position(x * 8, y * 8).getDistance(TerrainTracker::Instance().getPlayerStartingPosition());
			if (GridTracker::Instance().getAntiMobilityMiniGrid(x, y) == 0 && GridTracker::Instance().getEnemyMiniGrd(x, y) == 0.0 && GridTracker::Instance().getMobilityMiniGrid(x, y) * distanceTarget / distanceHome > highestMobility)
			{
				bool safeTile = true;
				for (int i = x - unit->getType().width() / 4; i <= x + unit->getType().width() / 4; i++)
				{
					for (int j = y - unit->getType().height() / 4; j <= y + unit->getType().height() / 4; j++)
					{
						if (GridTracker::Instance().getEnemyMiniGrd(i, j) > 0.0 || GridTracker::Instance().getAntiMobilityMiniGrid(i, j) > 0)
						{
							safeTile = false;
						}
					}
				}
				if (safeTile)
				{
					highestMobility = GridTracker::Instance().getMobilityMiniGrid(x, y) * distanceTarget / distanceHome;
					finalPosition = WalkPosition(x, y);
				}
			}
		}
	}
	GridTracker::Instance().updateAllyMovement(unit, finalPosition);
	return Position(finalPosition);
}

void CommandTrackerClass::arbiterManager(Unit unit)
{
	if (GridTracker::Instance().getSupportPosition() != Positions::None && GridTracker::Instance().getSupportPosition() != Positions::Unknown && GridTracker::Instance().getSupportPosition() != Positions::Invalid)
	{
		unit->move(GridTracker::Instance().getSupportPosition());
	}
	else
	{
		unit->move(TerrainTracker::Instance().getPlayerStartingPosition());
	}
	if (unit->getUnitsInRadius(640, Filter::IsEnemy).size() > 4)
	{
		Unit target = UnitTracker::Instance().getMyUnits()[unit].getTarget();
		if (target)
		{
			unit->useTech(TechTypes::Stasis_Field, target);
		}
	}
}

void CommandTrackerClass::templarManager(Unit unit)
{

}
