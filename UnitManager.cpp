#include "UnitManager.h"
#include "TerrainManager.h"
#include "GridManager.h"
#include "SpecialUnitManager.h"
#include "ProbeManager.h"
#include "BuildingManager.h"
#include "UnitUtil.h"
#include "StrategyManager.h"
#include "NexusManager.h"
#include "TargetManager.h"
#include "PylonManager.h"

void UnitTrackerClass::update()
{
	storeUnits();
	removeUnits();
}

void UnitTrackerClass::storeUnits()
{
	// Reset sizes and supply
	for (auto &size : allySizes)
	{
		size.second = 0;
	}
	for (auto &size : enemySizes)
	{
		size.second = 0;
	}
	supply = 0;

	// For all ally units
	for (auto &u : Broodwar->self()->getUnits())
	{
		// Add supply of this unit
		if (u->getType().supplyRequired() > 0)
		{
			supply = supply + u->getType().supplyRequired();
		}

		// Don't want to store scarabs or units that don't exist
		if (u->getType() == UnitTypes::Protoss_Scarab || !u || !u->exists())
		{
			continue;
		}

		// Store buildings even if they're not completed
		if (u->getType().isBuilding())
		{
			BuildingTracker::Instance().storeBuilding(u);
			if (u->getType() == Protoss_Nexus)
			{
				NexusTracker::Instance().storeNexus(u);
			}
			else if (u->getType() == Protoss_Pylon)
			{
				PylonTracker::Instance().storePylon(u);
			}
		}		

		// Don't want to store units that aren't completed
		else if (!u->isCompleted())
		{
			continue;
		}

		// Store Probes
		else if (u->getType() == UnitTypes::Protoss_Probe)
		{
			ProbeTracker::Instance().storeProbe(u);
		}
		// Store Special Units in both
		else if (u->getType() == UnitTypes::Protoss_Observer || u->getType() == UnitTypes::Protoss_Reaver || u->getType() == UnitTypes::Protoss_High_Templar || u->getType() == UnitTypes::Protoss_Arbiter || u->getType() == UnitTypes::Protoss_Shuttle)
		{
			storeAllyUnit(u);
			SpecialUnitTracker::Instance().storeUnit(u);
		}
		// Store the rest
		else
		{
			storeAllyUnit(u);
		}
	}

	// For all enemy units
	for (auto player : Broodwar->enemies())
	{
		for (auto &u : player->getUnits())
		{
			// Store if exists
			if (u && u->exists())
			{
				storeEnemyUnit(u);
			}
		}
	}
}

void UnitTrackerClass::removeUnits()
{
	// Check for decayed ally units
	for (map<Unit, UnitInfoClass>::iterator itr = allyUnits.begin(); itr != allyUnits.end();)
	{
		if (itr->second.getDeadFrame() != 0 && itr->second.getDeadFrame() + 500 < Broodwar->getFrameCount())
		{
			itr = allyUnits.erase(itr);
		}
		else
		{
			++itr;
		}
	}
	// Check for decayed enemy units
	for (map<Unit, UnitInfoClass>::iterator itr = enemyUnits.begin(); itr != enemyUnits.end();)
	{
		if ((*itr).second.getDeadFrame() != 0 && (*itr).second.getDeadFrame() + 500 < Broodwar->getFrameCount() || itr->first && itr->first->exists() && itr->first->getPlayer() != Broodwar->enemy())
		{
			itr = enemyUnits.erase(itr);
		}
		else
		{
			++itr;
		}
	}
}

void UnitTrackerClass::storeEnemyUnit(Unit unit)
{
	// Update units
	enemyUnits[unit].setUnitType(unit->getType());
	enemyUnits[unit].setPosition(unit->getPosition());
	enemyUnits[unit].setStrength(UnitUtil::Instance().getVisibleStrength(unit));
	enemyUnits[unit].setMaxStrength(UnitUtil::Instance().getStrength(unit->getType()));
	enemyUnits[unit].setGroundRange(UnitUtil::Instance().getTrueRange(unit->getType(), unit->getPlayer()));
	enemyUnits[unit].setAirRange(UnitUtil::Instance().getTrueAirRange(unit->getType(), unit->getPlayer()));
	enemyUnits[unit].setSpeed(UnitUtil::Instance().getTrueSpeed(unit->getType(), Broodwar->self()));
	enemyUnits[unit].setPriority(UnitUtil::Instance().getPriority(unit->getType()));
	enemyUnits[unit].setGroundDamage(UnitUtil::Instance().getTrueGroundDamage(unit->getType(), unit->getPlayer()));
	enemyUnits[unit].setAirDamage(UnitUtil::Instance().getTrueAirDamage(unit->getType(), unit->getPlayer()));
	enemyUnits[unit].setCommand(unit->getLastCommand().getType());
	enemyUnits[unit].setMiniTile(UnitUtil::Instance().getMiniTile(unit));

	// Update sizes
	enemySizes[unit->getType().size()] += 1;
	return;
}

void UnitTrackerClass::storeAllyUnit(Unit unit)
{
	// Update units
	allyUnits[unit].setUnitType(unit->getType());
	allyUnits[unit].setPosition(unit->getPosition());
	allyUnits[unit].setStrength(UnitUtil::Instance().getVisibleStrength(unit));
	allyUnits[unit].setMaxStrength(UnitUtil::Instance().getStrength(unit->getType()));
	allyUnits[unit].setGroundRange(UnitUtil::Instance().getTrueRange(unit->getType(), unit->getPlayer()));
	allyUnits[unit].setAirRange(UnitUtil::Instance().getTrueAirRange(unit->getType(), unit->getPlayer()));
	allyUnits[unit].setSpeed(UnitUtil::Instance().getTrueSpeed(unit->getType(), Broodwar->self()));
	allyUnits[unit].setPriority(UnitUtil::Instance().getPriority(unit->getType()));
	allyUnits[unit].setGroundDamage(UnitUtil::Instance().getTrueGroundDamage(unit->getType(), unit->getPlayer()));
	allyUnits[unit].setAirDamage(UnitUtil::Instance().getTrueAirDamage(unit->getType(), unit->getPlayer()));
	allyUnits[unit].setCommand(unit->getLastCommand().getType());
	allyUnits[unit].setMiniTile(UnitUtil::Instance().getMiniTile(unit));
	allyUnits[unit].setTarget(TargetTracker::Instance().getTarget(unit));
	allyUnits[unit].setTargetPosition(enemyUnits[allyUnits[unit].getTarget()].getPosition());

	// Update sizes and calculations
	allySizes[unit->getType().size()] += 1;
	getGlobalCalculation(unit, allyUnits[unit].getTarget());
	getLocalCalculation(unit, allyUnits[unit].getTarget());
	return;
}

void UnitTrackerClass::decayUnit(Unit unit)
{
	if (allyUnits.find(unit) != allyUnits.end() && unit->getPlayer() == Broodwar->self())
	{
		allyUnits[unit].setDeadFrame(Broodwar->getFrameCount());
	}
	else if (enemyUnits.find(unit) != enemyUnits.end())
	{
		enemyUnits[unit].setDeadFrame(Broodwar->getFrameCount());
	}
}

void UnitTrackerClass::getLocalCalculation(Unit unit, Unit target)
{
	// Variables for calculating local strengths
	double enemyLocalStrength = 0.0, allyLocalStrength = 0.0, thisUnit = 0.0;
	Position targetPosition = allyUnits[unit].getTargetPosition();

	int aLarge = getMySizes()[UnitSizeTypes::Large];
	int aMedium = getMySizes()[UnitSizeTypes::Medium];
	int aSmall = getMySizes()[UnitSizeTypes::Small];

	int eLarge = getEnSizes()[UnitSizeTypes::Large];
	int eMedium = getEnSizes()[UnitSizeTypes::Medium];
	int eSmall = getEnSizes()[UnitSizeTypes::Small];

	// Reset local
	allyUnits[unit].setLocal(0);

	if (unit->getDistance(targetPosition) > 640)
	{
		allyUnits[unit].setStrategy(3);
		return;
	}

	// Check every enemy unit being in range of the target
	for (auto &u : enemyUnits)
	{
		// Ignore workers, keep buildings (reinforcements and static defenses)
		if (u.second.getType().isWorker() || (u.first && u.first->exists() && u.first->isStasised()))
		{
			continue;
		}

		// Reset unit strength
		thisUnit = 0.0;
		double threatRange = (u.second.getGroundRange() / 32.0) + u.second.getSpeed() + double(u.second.getType().width()) / 32 + double(allyUnits[target].getType().width()) / 32;

		// If a unit is within range of the target, add to local strength
		if (u.second.getPosition().getDistance(targetPosition) / 32.0 < threatRange)
		{
			if (aLarge > 0 || aMedium > 0 || aSmall > 0)
			{
				// If unit is cloaked or burrowed and not detected, drastically increase strength
				if ((u.first->isCloaked() || u.first->isBurrowed()) && !u.first->isDetected())
				{
					thisUnit = 20 * u.second.getMaxStrength();
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
	for (auto &u : allyUnits)
	{
		// Ignore workers and buildings
		if (u.second.getType().isWorker() || u.second.getType().isBuilding())
		{
			continue;
		}

		// Reset unit strength
		thisUnit = 0.0;
		double threatRange = (u.second.getGroundRange() / 32.0) + u.second.getSpeed() + double(u.second.getType().width()) / 32.0 + double(enemyUnits[target].getType().width()) / 32;

		// If a unit is within the range of the ally unit, add to local strength
		if (u.second.getPosition().getDistance(unit->getPosition()) / 32.0 < threatRange)
		{
			if (eLarge > 0 || eMedium > 0 || eSmall > 0)
			{
				// If shuttle, add units inside
				if (u.second.getType() == UnitTypes::Protoss_Shuttle && u.first->getLoadedUnits().size() > 0)
				{
					// Assume reaver for damage type calculations
					for (Unit uL : u.first->getLoadedUnits())
					{
						thisUnit = allyUnits[uL].getStrength();
					}
				}
				else
				{
					// Damage type calculations
					if (u.second.getType().groundWeapon().damageType() == DamageTypes::Explosive)
					{
						thisUnit = u.second.getStrength() * ((eLarge*1.0) + (eMedium*0.75) + (eSmall*0.5)) / (eLarge + eMedium + eSmall);
					}
					else if (u.second.getType().groundWeapon().damageType() == DamageTypes::Concussive)
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
	allyUnits[unit].setLocal(allyLocalStrength - enemyLocalStrength);

	// Specific High Templar behavior
	if (unit->getType() == Protoss_High_Templar)
	{
		if (unit->getEnergy() > 75)
		{
			allyUnits[unit].setStrategy(1);
			return;
		}
		else
		{
			allyUnits[unit].setStrategy(0);
			return;
		}
	}

	// Specific Zealot Commands
	if (unit->getType() == UnitTypes::Protoss_Zealot)
	{
		// Force Zealots to stay on Tanks and Reavers to shoot if units are near it
		if (target && target->exists() && (enemyUnits[target].getType() == UnitTypes::Terran_Siege_Tank_Siege_Mode || enemyUnits[target].getType() == UnitTypes::Terran_Siege_Tank_Tank_Mode) && unit->getDistance(targetPosition) < 128)
		{
			allyUnits[unit].setStrategy(1);
			return;
		}

		// Check if we are still in defensive mode
		if (globalStrategy == 2)
		{
			// If against rush and not ready to wall up, fight in mineral line
			if (StrategyTracker::Instance().isRush() && !StrategyTracker::Instance().needZealotWall())
			{
				if (target && target->exists() && GridTracker::Instance().getResourceGrid(target->getTilePosition().x, target->getTilePosition().y) > 0)
				{
					allyUnits[unit].setStrategy(1);
					return;
				}
				else
				{
					allyUnits[unit].setStrategy(2);
					return;
				}
			}

			// Else hold ramp and attack anything within range
			else if (StrategyTracker::Instance().needZealotWall())
			{
				if (target && target->exists() && unit->getDistance(target) < 16)
				{
					allyUnits[unit].setStrategy(1);
					return;
				}
				else
				{
					allyUnits[unit].setStrategy(2);
					return;
				}
			}
		}
	}

	// Specific Dragoon commands for early defense
	if (globalStrategy == 2 && unit->getType() == UnitTypes::Protoss_Dragoon)
	{
		if (target && target->exists() && (unit->getDistance(target) <= allyUnits[unit].getGroundRange() || (TerrainTracker::Instance().getAllyTerritory().find(getRegion(target->getTilePosition())) != TerrainTracker::Instance().getAllyTerritory().end() && !target->getType().isWorker())))
		{
			allyUnits[unit].setStrategy(1);
			return;
		}
		else
		{
			allyUnits[unit].setStrategy(2);
			return;
		}
	}

	// If an enemy is within ally territory, engage
	if (target && target->exists() && TerrainTracker::Instance().getAllyTerritory().find(getRegion(target->getTilePosition())) != TerrainTracker::Instance().getAllyTerritory().end())
	{
		allyUnits[unit].setStrategy(1);
		return;
	}

	// If a unit has higher range than another unit and is currently safe, engage it
	if (allyUnits[unit].getGroundRange() > enemyUnits[target].getGroundRange())
	{
		bool safeTile = true;
		for (auto miniTile : UnitUtil::Instance().getMiniTilesUnderUnit(unit))
		{
			if (miniTile.isValid() && GridTracker::Instance().getEDistanceGrid(miniTile.x, miniTile.y) > 0)
			{
				safeTile = false;
			}
		}
		if (safeTile)
		{
			allyUnits[unit].setStrategy(1);
			return;
		}
	}

	// If a Reaver is in range of something, engage it
	if (unit->getType() == UnitTypes::Protoss_Reaver && allyUnits[unit].getGroundRange() > unit->getDistance(targetPosition))
	{
		allyUnits[unit].setStrategy(1);
		return;
	}

	// If last command was engage
	if (allyUnits[unit].getStrategy() == 1)
	{
		// Latch based system for at least 80% disadvantage to disengage
		if (allyLocalStrength < enemyLocalStrength*0.8)
		{
			allyUnits[unit].setStrategy(0);
			return;
		}
		allyUnits[unit].setStrategy(1);
		return;
	}
	// If last command was disengage/no command
	else
	{
		// Latch based system for at least 120% advantage to engage
		if (allyLocalStrength >= enemyLocalStrength*1.2)
		{
			allyUnits[unit].setStrategy(1);
			return;
		}
		// Otherwise return 3 or 0, whichever was previous
		allyUnits[unit].setStrategy(0);
		return;
	}
	// Disregard local if no target, no recent local calculation and not within ally region
	allyUnits[unit].setStrategy(3);
	return;
}

void UnitTrackerClass::getGlobalCalculation(Unit unit, Unit target)
{
	if (StrategyTracker::Instance().isFastExpand())
	{
		globalStrategy = 0;
		return;
	}

	// If Zerg, wait for a larger army before moving out
	if (Broodwar->enemy()->getRace() == Races::Zerg && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Shuttle) < 1)
	{
		globalStrategy = 2;
		return;
	}
	// If Toss, wait for Dragoon range upgrade
	if (Broodwar->enemy()->getRace() == Races::Protoss && Broodwar->self()->getUpgradeLevel(UpgradeTypes::Singularity_Charge) == 0)
	{
		globalStrategy = 2;
		return;
	}

	if (StrategyTracker::Instance().globalAlly() > StrategyTracker::Instance().globalEnemy())
	{
		globalStrategy = 1;
		return;
	}
	else
	{
		// If Terran, contain
		if (Broodwar->enemy()->getRace() == Races::Terran)
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