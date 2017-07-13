#include "McRave.h"

void UnitTrackerClass::update()
{
	Display().startClock();
	updateAliveUnits();
	updateDeadUnits();
	updateGlobalCalculations();
	Display().performanceTest(__FUNCTION__);
	return;
}

void UnitTrackerClass::updateAliveUnits()
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
				updateAlly(u);
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
			updateAlly(u);
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
				updateEnemy(u);
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

	// TESTING -- Calculate how a unit is performing
	for (auto& bullet : Broodwar->getBullets())
	{
		if (bullet->exists() && bullet->getSource() && bullet->getSource()->exists() && bullet->getTarget() && bullet->getTarget()->exists())
		{
			if (bullet->getSource()->getPlayer() == Broodwar->self() && myBullets.find(bullet) == myBullets.end())
			{
				myBullets.emplace(bullet);
				double typeMod = 1.0;

				if (!bullet->getTarget()->getType().isFlyer())
				{
					if (bullet->getSource()->getType().groundWeapon().damageType() == DamageTypes::Explosive)
					{
						if (bullet->getTarget()->getType().size() == UnitSizeTypes::Small)
						{
							typeMod = 0.5;
						}
						if (bullet->getTarget()->getType().size() == UnitSizeTypes::Medium)
						{
							typeMod = 0.75;
						}
					}
					if (bullet->getSource()->getType().groundWeapon().damageType() == DamageTypes::Concussive)
					{
						if (bullet->getTarget()->getType().size() == UnitSizeTypes::Large)
						{
							typeMod = 0.25;
						}
						if (bullet->getTarget()->getType().size() == UnitSizeTypes::Medium)
						{
							typeMod = 0.5;
						}
					}

					unitPerformance[bullet->getSource()->getType()] += double(bullet->getSource()->getType().groundWeapon().damageAmount()) * typeMod;
				}
			}
			else
			{
				unitPerformance[bullet->getSource()->getType()] += bullet->getSource()->getType().airWeapon().damageAmount();
			}
		}
	}
}

void UnitTrackerClass::updateDeadUnits()
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

void UnitTrackerClass::updateEnemy(Unit unit)
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
	u.setVisibleGroundStrength(Util().getVisibleGroundStrength(u, p));
	u.setMaxGroundStrength(Util().getMaxGroundStrength(u, p));
	u.setVisibleAirStrength(Util().getVisibleAirStrength(u, p));
	u.setMaxAirStrength(Util().getMaxAirStrength(u, p));
	u.setPriority(Util().getPriority(u, p));
	enemySizes[t.size()] += 1;
	return;
}

void UnitTrackerClass::updateAlly(Unit unit)
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
	u.setVisibleGroundStrength(Util().getVisibleGroundStrength(u, p));
	u.setMaxGroundStrength(Util().getMaxGroundStrength(u, p));
	u.setVisibleAirStrength(Util().getVisibleAirStrength(u, p));
	u.setMaxAirStrength(Util().getMaxAirStrength(u, p));
	u.setPriority(Util().getPriority(u, p));
	allySizes[t.size()] += 1;

	if (unit->getLastCommand().getTargetPosition().isValid())
	{
		u.setTargetPosition(unit->getLastCommand().getTargetPosition());
	}

	// Update calculations
	u.setTarget(Targets().getTarget(u));
	getLocalCalculation(u);
	return;
}

void UnitTrackerClass::removeUnit(Unit unit)
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

void UnitTrackerClass::getLocalCalculation(UnitInfo& unit) // Will eventually be moved to UTIL as a double function, to be accesible to SpecialUnitManager too
{
	// Variables for calculating local strengths
	double enemyLocalGroundStrength = 0.0, allyLocalGroundStrength = 0.0, timeToTarget = 0.0;
	double enemyLocalAirStrength = 0.0, allyLocalAirStrength = 0.0;
	Position engagementPosition = unit.getPosition();

	// Reset local
	unit.setGroundLocal(0);

	// Time to reach target
	if (unit.getPosition().getDistance(unit.getTargetPosition()) > unit.getGroundRange() && unit.getSpeed() > 0.0)
	{
		timeToTarget = (unit.getPosition().getDistance(unit.getTargetPosition()) - unit.getGroundRange()) / unit.getSpeed();		
	}

	if (unit.getPosition().getDistance(unit.getTargetPosition()) > 640.0)
	{
		unit.setStrategy(3);
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

		// If a unit is within threat range of the target, add to local strength		
		if (enemy.getGroundDamage() > 0 && enemy.getPosition().getDistance(unit.getTargetPosition()) < max(unit.getGroundRange(), enemy.getGroundRange() + (enemy.getSpeed() * timeToTarget)))
		{
			// If enemy hasn't died, add to enemy. Otherwise, partially add to ally local
			if (enemy.getDeadFrame() == 0)
			{				
				enemyLocalGroundStrength += enemy.getVisibleGroundStrength();
			}
			else
			{
				allyLocalGroundStrength += enemy.getMaxGroundStrength() * 1.0 / (1.0 + 0.001*(double(Broodwar->getFrameCount()) - double(enemy.getDeadFrame())));
			}
		}
		if (enemy.getAirDamage() > 0 && enemy.getPosition().getDistance(unit.getTargetPosition()) < max(unit.getAirRange(), enemy.getAirRange() + (enemy.getSpeed() * timeToTarget)))
		{
			// If enemy hasn't died, add to enemy. Otherwise, partially add to ally local
			if (enemy.getDeadFrame() == 0)
			{
				enemyLocalAirStrength += enemy.getVisibleAirStrength();
			}
			else
			{
				allyLocalAirStrength += enemy.getMaxAirStrength() * 1.0 / (1.0 + 0.001*(double(Broodwar->getFrameCount()) - double(enemy.getDeadFrame())));
			}
		}

	}

	// Check every ally being in range of the target
	for (auto &a : allyUnits)
	{
		UnitInfo ally = a.second;
		// Ignore workers and buildings
		if (ally.getType().isWorker() || ally.getType().isBuilding() || ally.unit() == unit.unit())
		{
			continue;
		}

		// If a unit is within threat range of the ally unit, add to local strength
		if (ally.getGroundDamage() > 0 && ally.getPosition().getDistance(unit.getPosition()) <  max(unit.getGroundRange(), ally.getGroundRange() + (ally.getSpeed() * timeToTarget)))
		{
			// If ally hasn't died, add to ally. Otherwise, partially add to enemy local
			if (ally.getDeadFrame() == 0)
			{
				allyLocalGroundStrength += ally.getVisibleGroundStrength();
			}
			else
			{
				enemyLocalGroundStrength += ally.getMaxGroundStrength() * 1.0 / (1.0 + 0.001*(double(Broodwar->getFrameCount()) - double(ally.getDeadFrame())));
			}
		}
		if (ally.getAirDamage() > 0 && ally.getPosition().getDistance(unit.getPosition()) <  max(unit.getAirRange(), ally.getAirRange() + (ally.getSpeed() * timeToTarget)))
		{
			// If enemy hasn't died, add to enemy. Otherwise, partially add to ally local
			if (ally.getDeadFrame() == 0)
			{
				allyLocalAirStrength += ally.getVisibleAirStrength();
			}
			else
			{
				enemyLocalAirStrength += ally.getMaxAirStrength() * 1.0 / (1.0 + 0.001*(double(Broodwar->getFrameCount()) - double(ally.getDeadFrame())));
			}
		}
	}

	// Future position of allySpecialUnits iterator

	// Store the difference of strengths 
	unit.setGroundLocal(allyLocalGroundStrength - enemyLocalGroundStrength);
	unit.setAirLocal(allyLocalAirStrength - enemyLocalAirStrength);

	// Specific High Templar strategy
	if (unit.getType() == UnitTypes::Protoss_High_Templar)
	{
		if (unit.unit()->getEnergy() < 75)
		{
			unit.setStrategy(0);
			return;
		}
	}

	// Specific Medic strategy
	if (unit.getType() == UnitTypes::Terran_Medic)
	{
		if (unit.unit()->getEnergy() <= 0)
		{
			unit.setStrategy(0);
			return;
		}
	}

	// Specific ground unit strategy
	if (!unit.getType().isFlyer())
	{
		// Specific melee strategy
		if (unit.getGroundRange() <= 32)
		{
			// Force to stay on tanks
			if (unit.getTarget() && unit.getTarget()->exists())
			{
				if ((unit.getTarget()->getType() == UnitTypes::Terran_Siege_Tank_Siege_Mode || unit.getTarget()->getType() == UnitTypes::Terran_Siege_Tank_Tank_Mode) && unit.getPosition().getDistance(unit.getTargetPosition()) < 128)
				{
					unit.setStrategy(1);
					return;
				}


				// Avoid attacking mines
				if (unit.getTarget()->getType() == UnitTypes::Terran_Vulture_Spider_Mine)
				{
					unit.setStrategy(0);
					return;
				}
			}

			// Check if we are still in defensive mode
			if (globalStrategy == 2)
			{
				// If against rush and not ready to wall up, fight in mineral line
				if (Strategy().isRush() && !Strategy().isHoldRamp())
				{
					if (unit.getTarget() && unit.getTarget()->exists() && (Grids().getResourceGrid(unit.getTarget()->getTilePosition()) > 0 || Grids().getResourceGrid(unit.getTilePosition()) > 0))
					{
						unit.setStrategy(1);
						return;
					}
					else
					{
						unit.setStrategy(2);
						return;
					}
				}

				// Else hold ramp and attack anything within range
				else if (Strategy().isHoldRamp())
				{
					if (unit.getTarget() && unit.getTarget()->exists() && unit.getPosition().getDistance(unit.getTargetPosition()) < 16 && Terrain().isInAllyTerritory(unit.getTarget()) && Terrain().isInAllyTerritory(unit.unit()) && !unit.getTarget()->getType().isWorker())
					{
						unit.setStrategy(1);
						return;
					}
					else
					{
						unit.setStrategy(2);
						return;
					}
				}
			}
		}

		// Specific ranged strategy
		else if (globalStrategy == 2 && unit.getGroundRange() > 32)
		{
			if (unit.getTarget() && unit.getTarget()->exists() && ((Terrain().isInAllyTerritory(unit.unit()) && unit.getPosition().getDistance(unit.getTargetPosition()) <= unit.getGroundRange()) || (Terrain().isInAllyTerritory(unit.getTarget()) && !unit.getTarget()->getType().isWorker())))
			{
				unit.setStrategy(1);
				return;
			}
			else
			{
				unit.setStrategy(2);
				return;
			}
		}

		// If an enemy is within ally territory, engage
		if (unit.getTarget() && unit.getTarget()->exists() && theMap.GetArea(unit.getTarget()->getTilePosition()) && Terrain().isInAllyTerritory(unit.getTarget()))
		{
			unit.setStrategy(1);
			return;
		}

		// If a Reaver is in range of something, engage it
		if (unit.getType() == UnitTypes::Protoss_Reaver && unit.getGroundRange() > unit.getPosition().getDistance(unit.getTargetPosition()))
		{
			unit.setStrategy(1);
			return;
		}
	}	

	// If last command was engage
	if (unit.getStrategy() == 1)
	{
		// Latch based system for at least 80% disadvantage to disengage
		if ((!unit.getType().isFlyer() && allyLocalGroundStrength < enemyLocalGroundStrength*0.8) || (unit.getType().isFlyer() && allyLocalAirStrength < enemyLocalAirStrength*0.8))
		{
			unit.setStrategy(0);
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
		if ((!unit.getType().isFlyer() && allyLocalGroundStrength > enemyLocalGroundStrength*1.2) || (unit.getType().isFlyer() && allyLocalAirStrength > enemyLocalAirStrength*1.2))
		{
			unit.setStrategy(1);
			return;
		}
		else
		{
			unit.setStrategy(0);
			return;
		}
	}
	// Disregard local if no target, no recent local calculation and not within ally region
	unit.setStrategy(3);
	return;
}

void UnitTrackerClass::updateGlobalCalculations()
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