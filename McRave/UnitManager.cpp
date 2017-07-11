#include "McRave.h"

void UnitTrackerClass::update()
{
	Display().startClock();
	storeUnits();
	removeUnits();
	Display().performanceTest(__FUNCTION__);
	return;
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

	// Store all allied units
	for (auto &u : Broodwar->self()->getUnits())
	{
		// Don't want to store scarabs or units that don't exist
		if (u->getType() == UnitTypes::Protoss_Scarab || !u || !u->exists())
		{
			continue;
		}

		// Add supply of this unit if it has one
		if (u->getType().supplyRequired() > 0)
		{
			supply = supply + u->getType().supplyRequired();
		}

		// Store buildings even if they're not completed
		if (u->getType().isBuilding())
		{
			Buildings().storeBuilding(u);
			if (u->getType().isResourceDepot())
			{
				Bases().storeBase(u);
			}
			else if (u->getType() == UnitTypes::Protoss_Pylon)
			{
				Pylons().storePylon(u);
			}
			else if (u->getType() == UnitTypes::Protoss_Shield_Battery)
			{
				Buildings().storeBattery(u);
			}
			else if (u->getType() == UnitTypes::Protoss_Photon_Cannon)
			{
				storeAllyUnit(u);
			}
		}

		// Don't want to store units that aren't completed
		else if (!u->isCompleted())
		{
			continue;
		}

		// Store workers
		else if (u->getType().isWorker())
		{
			Workers().storeWorker(u);
		}
		// Store units
		else
		{
			storeAllyUnit(u);
			if (u->getType() == UnitTypes::Protoss_Observer || u->getType() == UnitTypes::Protoss_Reaver || u->getType() == UnitTypes::Protoss_High_Templar || u->getType() == UnitTypes::Protoss_Arbiter || u->getType() == UnitTypes::Terran_Medic)
			{
				SpecialUnits().storeUnit(u);
			}
			else if (u->getType() == UnitTypes::Protoss_Shuttle)
			{
				Transport().storeUnit(u);
			}
		}
	}

	// Store all enemy units
	for (auto &player : Broodwar->enemies())
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

	// Store all neutral units
	for (auto &r : Broodwar->neutral()->getUnits())
	{
		if (r && r->exists())
		{
			if (Grids().getBaseGrid(r->getTilePosition()) != 0)
			{
				if (r->getType().isMineralField() && r->getInitialResources() > 0 && Resources().getMyMinerals().find(r) == Resources().getMyMinerals().end())
				{
					Resources().storeMineral(r);
				}

				if (Resources().getMyGas().find(r) == Resources().getMyGas().end() && r->getType() == UnitTypes::Resource_Vespene_Geyser)
				{
					Resources().storeGas(r);
				}
			}
			else if (Grids().getBaseGrid(r->getTilePosition()) == 0)
			{
				Resources().removeResource(r);
			}
			if (r->getInitialResources() == 0 && r->getDistance(Terrain().getPlayerStartingPosition()) < 2560)
			{
				Resources().storeBoulder(r);
			}
		}
	}
}

void UnitTrackerClass::removeUnits()
{
	// Check for decayed ally units
	for (map<Unit, UnitInfo>::iterator itr = allyUnits.begin(); itr != allyUnits.end();)
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
	for (map<Unit, UnitInfo>::iterator itr = enemyUnits.begin(); itr != enemyUnits.end();)
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
	auto &u = enemyUnits[unit];
	auto t = unit->getType();
	auto p = unit->getPlayer();

	// Update information
	u.setUnit(unit);
	u.setUnitType(t);
	u.setPosition(unit->getPosition());
	u.setWalkPosition(Util().getWalkPosition(unit));
	u.setTilePosition(unit->getTilePosition());
	u.setPlayer(unit->getPlayer());
	
	// Update statistics
	u.setGroundRange(Util().getTrueRange(t, p));
	u.setAirRange(Util().getTrueAirRange(t, p));	
	u.setGroundDamage(Util().getTrueGroundDamage(t, p));
	u.setAirDamage(Util().getTrueAirDamage(t, p));
	u.setSpeed(Util().getTrueSpeed(t, p));
	u.setMinStopFrame(Util().getMinStopFrame(t));	

	// Update sizes and strength
	u.setStrength(Util().getVisibleStrength(u, p));
	u.setMaxStrength(Util().getStrength(u, p));
	u.setPriority(Util().getPriority(u, p));
	enemySizes[t.size()] += 1;
	return;
}

void UnitTrackerClass::storeAllyUnit(Unit unit)
{
	auto &u = allyUnits[unit];
	auto t = unit->getType();
	auto p = unit->getPlayer();

	// Update information
	u.setUnit(unit);
	u.setUnitType(t);
	u.setPosition(unit->getPosition());
	u.setTilePosition(unit->getTilePosition());
	u.setWalkPosition(Util().getWalkPosition(unit));
	u.setPlayer(unit->getPlayer());

	// Update statistics
	u.setGroundRange(Util().getTrueRange(t, p));
	u.setAirRange(Util().getTrueAirRange(t, p));
	u.setGroundDamage(Util().getTrueGroundDamage(t, p));
	u.setAirDamage(Util().getTrueAirDamage(t, p));
	u.setSpeed(Util().getTrueSpeed(t, p));
	u.setMinStopFrame(Util().getMinStopFrame(t));

	// Update sizes and strength
	u.setStrength(Util().getVisibleStrength(u, p));
	u.setMaxStrength(Util().getStrength(u, p));
	u.setPriority(Util().getPriority(u, p));
	allySizes[t.size()] += 1;

	// Update calculations
	u.setTarget(Targets().getTarget(u));
	getGlobalCalculation(unit, allyUnits[unit].getTarget());
	getLocalCalculation(unit, allyUnits[unit].getTarget());
	return;
}

void UnitTrackerClass::decayUnit(Unit unit)
{
	if (allyUnits.find(unit) != allyUnits.end())
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
	double timeToTarget = 0.0;
	
	// Time to reach target
	if (unit->getDistance(targetPosition) > allyUnits[unit].getGroundRange() && allyUnits[unit].getSpeed() > 0)
	{
		timeToTarget = (unit->getDistance(targetPosition) - allyUnits[unit].getGroundRange()) / allyUnits[unit].getSpeed();
	}

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
	for (auto &e : enemyUnits)
	{
		UnitInfo enemy = e.second;
		// Ignore workers and stasised units
		if (enemy.getType().isWorker() || (enemy.unit() && enemy.unit()->exists() && enemy.unit()->isStasised()))
		{
			continue;
		}

		// Reset unit strength
		thisUnit = 0.0;

		double threatRange = enemy.getGroundRange() + (enemy.getSpeed() * timeToTarget);


		// If a unit is within range of the target, add to local strength
		if (enemy.getPosition().getDistance(targetPosition) < threatRange)
		{
			if (aLarge > 0 || aMedium > 0 || aSmall > 0)
			{
				// If unit is cloaked or burrowed and not detected, drastically increase strength
				if ((enemy.unit()->isCloaked() || enemy.unit()->isBurrowed()) && !enemy.unit()->isDetected())
				{
					thisUnit = 20.0 * enemy.getMaxStrength();
				}
				else if (enemy.getType().groundWeapon().damageType() == DamageTypes::Explosive)
				{
					thisUnit = enemy.getStrength() * double((aLarge*1.0) + (aMedium*0.75) + (aSmall*0.5)) / double(aLarge + aMedium + aSmall);
				}
				else if (enemy.getType().groundWeapon().damageType() == DamageTypes::Concussive)
				{
					thisUnit = enemy.getStrength() * double((aLarge*1.0) + (aMedium*0.75) + (aSmall*0.5)) / double(aLarge + aMedium + aSmall);
				}
				else
				{
					thisUnit = enemy.getStrength();
				}
			}
			else
			{
				thisUnit = enemy.getStrength();
			}
			// If enemy hasn't died, add to enemy. Otherwise, partially add to ally local
			if (enemy.getDeadFrame() == 0)
			{
				enemyLocalStrength += thisUnit;
			}
			else
			{
				allyLocalStrength += enemy.getMaxStrength() * 1.0 / (1.0 + 0.001*(double(Broodwar->getFrameCount()) - double(enemy.getDeadFrame())));
			}
		}
	}

	// Check every ally being in range of the target
	for (auto &a : allyUnits)
	{
		UnitInfo ally = a.second;
		// Ignore workers and buildings
		if (ally.getType().isWorker() || ally.getType().isBuilding())
		{
			continue;
		}

		// Reset unit strength
		thisUnit = 0.0;

		double threatRange = ally.getGroundRange() + (ally.getSpeed() * timeToTarget);


		// If a unit is within the range of the ally unit, add to local strength
		if (ally.getPosition().getDistance(unit->getPosition()) < threatRange)
		{
			if (eLarge > 0 || eMedium > 0 || eSmall > 0)
			{
				// If shuttle, add units inside
				if (ally.getType() == UnitTypes::Protoss_Shuttle && ally.unit()->getLoadedUnits().size() > 0)
				{
					// Assume reaver for damage type calculations
					for (Unit loadedUnit : ally.unit()->getLoadedUnits())
					{
						thisUnit = allyUnits[loadedUnit].getStrength();
					}
				}
				else
				{
					// Damage type calculations
					if (ally.getType().groundWeapon().damageType() == DamageTypes::Explosive)
					{
						thisUnit = ally.getStrength() * double ((eLarge*1.0) + (eMedium*0.75) + (eSmall*0.5)) / double (eLarge + eMedium + eSmall);
					}
					else if (ally.getType().groundWeapon().damageType() == DamageTypes::Concussive)
					{
						thisUnit = ally.getStrength() * double ((eLarge*1.0) + (eMedium*0.75) + (eSmall*0.5)) / double (eLarge + eMedium + eSmall);
					}
					else
					{
						thisUnit = ally.getStrength();
					}
				}
			}
			else
			{
				thisUnit = ally.getStrength();
			}

			// If ally hasn't died, add to ally. Otherwise, partially add to enemy local
			if (ally.getDeadFrame() == 0)
			{
				allyLocalStrength += thisUnit;
			}
			else
			{
				enemyLocalStrength += ally.getMaxStrength() * 1.0 / (1.0 + 0.001*(double(Broodwar->getFrameCount()) - double(ally.getDeadFrame())));
			}
		}
	}

	// Store the difference of strengths 
	allyUnits[unit].setLocal(allyLocalStrength - enemyLocalStrength);

	// Specific High Templar strategy
	if (unit->getType() == UnitTypes::Protoss_High_Templar)
	{
		if (unit->getEnergy() < 75)
		{
			allyUnits[unit].setStrategy(0);
			return;
		}		
	}

	// Specific Medic strategy
	if (unit->getType() == UnitTypes::Terran_Medic)
	{
		if (unit->getEnergy() <= 0)
		{
			allyUnits[unit].setStrategy(0);
			return;
		}
	}

	// Specific melee strategy

	if (allyUnits[unit].getGroundRange() <= 32)
	{
		// Force to stay on tanks
		if (target && target->exists() && (enemyUnits[target].getType() == UnitTypes::Terran_Siege_Tank_Siege_Mode || enemyUnits[target].getType() == UnitTypes::Terran_Siege_Tank_Tank_Mode) && unit->getDistance(targetPosition) < 128)
		{
			allyUnits[unit].setStrategy(1);
			return;
		}


		// Avoid attacking mines

		if (target && target->exists() && enemyUnits[target].getType() == UnitTypes::Terran_Vulture_Spider_Mine)
		{
			allyUnits[unit].setStrategy(0);
			return;
		}

		// Check if we are still in defensive mode
		if (globalStrategy == 2)
		{
			// If against rush and not ready to wall up, fight in mineral line
			if (Strategy().isRush() && !Strategy().isHoldRamp())
			{
				if (target && target->exists() && (Grids().getResourceGrid(target->getTilePosition()) > 0 || Grids().getResourceGrid(unit->getTilePosition()) > 0))
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
			else if (Strategy().isHoldRamp())
			{
				if (target && target->exists() && unit->getDistance(target) < 16 && Terrain().isInAllyTerritory(target) && Terrain().isInAllyTerritory(unit) && !target->getType().isWorker())
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


	// Specific ranged strategy

	else if (globalStrategy == 2 && allyUnits[unit].getGroundRange() > 32)
	{
		if (target && target->exists() && ((Terrain().isInAllyTerritory(unit) && unit->getDistance(target) <= allyUnits[unit].getGroundRange()) || (Terrain().isInAllyTerritory(target) && !target->getType().isWorker())))
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
	if (target && target->exists() && theMap.GetArea(target->getTilePosition()) && Terrain().isInAllyTerritory(target))
	{
		allyUnits[unit].setStrategy(1);
		return;
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
		else
		{
			return;
		}
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
		else
		{
			allyUnits[unit].setStrategy(0);
			return;
		}
	}
	// Disregard local if no target, no recent local calculation and not within ally region
	allyUnits[unit].setStrategy(3);
	return;
}

void UnitTrackerClass::getGlobalCalculation(Unit unit, Unit target)
{
	if (Broodwar->self()->getRace() == Races::Protoss)
	{
		if (Strategy().isFastExpand())
		{
			globalStrategy = 0;
			return;
		}

		// If Zerg, wait for a larger army before moving out
		if (Broodwar->enemy()->getRace() == Races::Zerg && Broodwar->self()->getUpgradeLevel(UpgradeTypes::Singularity_Charge) == 0)
		{
			globalStrategy = 2;
			return;
		}

		// If Toss, wait for Dragoon range upgrade against rushes
		if (Broodwar->enemy()->getRace() == Races::Protoss && Strategy().isRush() && Broodwar->self()->getUpgradeLevel(UpgradeTypes::Singularity_Charge) == 0)
		{
			globalStrategy = 2;
			return;
		}

		if (Strategy().globalAlly() > Strategy().globalEnemy())
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
	}
	else if (Broodwar->self()->getRace() == Races::Terran)
	{
		if (Broodwar->self()->completedUnitCount(UnitTypes::Terran_Medic) >= 2)
		{
			globalStrategy = 1;
			return;
		}
		else
		{
			globalStrategy = 0;
			return;
		}
	}
	globalStrategy = 1;
	return;
}