#include "McRave.h"

void ProductionTrackerClass::update()
{
	updateReservedResources();
	int supply = Units().getSupply();
	int queuedMineral = Buildings().getQueuedMineral();
	int queuedGas = Buildings().getQueuedGas();

	if (Broodwar->enemy()->getRace() == Races::Terran)
	{
		noZealots = true;
	}
	if (Broodwar->getFrameCount() > 10000)
	{
		noZealots = false;
	}

	if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Gateway) >= (2 * Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Nexus)))
	{
		gateSat = true;
	}

	for (auto &building : Buildings().getMyBuildings())
	{
		if (building.first->isIdle())
		{
			if (building.second.getUnitType() == UnitTypes::Protoss_Forge)
			{
				if (Broodwar->self()->minerals() >= UpgradeTypes::Protoss_Ground_Weapons.mineralPrice() + queuedMineral + reservedMineral && Broodwar->self()->gas() >= UpgradeTypes::Protoss_Ground_Weapons.gasPrice() + queuedGas + reservedGas)
				{
					building.first->upgrade(UpgradeTypes::Protoss_Ground_Weapons);
				}
				if (Broodwar->self()->minerals() >= UpgradeTypes::Protoss_Ground_Armor.mineralPrice() + queuedMineral + reservedMineral && Broodwar->self()->gas() >= UpgradeTypes::Protoss_Ground_Armor.gasPrice() + queuedGas + reservedGas)
				{
					building.first->upgrade(UpgradeTypes::Protoss_Ground_Armor);
				}
				if (Broodwar->self()->minerals() >= UpgradeTypes::Protoss_Plasma_Shields.mineralPrice() + queuedMineral + reservedMineral && Broodwar->self()->gas() >= UpgradeTypes::Protoss_Plasma_Shields.gasPrice() + queuedGas + reservedGas)
				{
					building.first->upgrade(UpgradeTypes::Protoss_Plasma_Shields);
				}
			}
			else if (building.second.getUnitType() == UnitTypes::Protoss_Cybernetics_Core)
			{
				if (!Broodwar->self()->getUpgradeLevel(UpgradeTypes::Singularity_Charge) && idleGates.size() == 0 && Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon) >= 3)
				{
					if (Broodwar->self()->minerals() >= UpgradeTypes::Singularity_Charge.mineralPrice() && Broodwar->self()->gas() >= UpgradeTypes::Singularity_Charge.gasPrice())
					{
						building.first->upgrade(UpgradeTypes::Singularity_Charge);
						idleUpgrade.erase(building.first);
					}
					else
					{
						idleUpgrade.emplace(building.first, UpgradeTypes::Singularity_Charge);
					}
				}
			}
			else if (building.second.getUnitType() == UnitTypes::Protoss_Robotics_Support_Bay)
			{
				if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Reaver) > 2)
				{
					if (Broodwar->enemy()->getRace() != Races::Zerg && Broodwar->self()->minerals() >= UpgradeTypes::Scarab_Damage.mineralPrice() + queuedMineral + reservedMineral && Broodwar->self()->gas() >= UpgradeTypes::Scarab_Damage.gasPrice() + queuedGas + reservedGas)
					{
						building.first->upgrade(UpgradeTypes::Scarab_Damage);
					}
				}
				if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Shuttle) >= 2)
				{
					if (Broodwar->self()->minerals() >= UpgradeTypes::Gravitic_Drive.mineralPrice() + queuedMineral + reservedMineral && Broodwar->self()->gas() >= UpgradeTypes::Gravitic_Drive.gasPrice() + queuedGas + reservedGas)
					{
						building.first->upgrade(UpgradeTypes::Gravitic_Drive);
					}
				}
			}
			else if (building.second.getUnitType() == UnitTypes::Protoss_Fleet_Beacon)
			{
				if (Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Carrier) > 2)
				{
					if (Broodwar->self()->minerals() >= UpgradeTypes::Carrier_Capacity.mineralPrice() + queuedMineral + reservedMineral && Broodwar->self()->gas() >= UpgradeTypes::Carrier_Capacity.gasPrice() + queuedGas + reservedGas)
					{
						building.first->upgrade(UpgradeTypes::Carrier_Capacity);
					}
				}
			}
			else if (building.second.getUnitType() == UnitTypes::Protoss_Citadel_of_Adun)
			{
				if (Broodwar->self()->minerals() >= UpgradeTypes::Leg_Enhancements.mineralPrice() && Broodwar->self()->gas() >= UpgradeTypes::Leg_Enhancements.gasPrice())
				{
					building.first->upgrade(UpgradeTypes::Leg_Enhancements);
					idleUpgrade.erase(building.first);
				}
				else
				{
					idleUpgrade.emplace(building.first, UpgradeTypes::Leg_Enhancements);
				}
			}

			// Production Buildings
			else if (building.second.getUnitType() == UnitTypes::Protoss_Gateway)
			{
				updateGateway(building.first);
			}
			else if (building.second.getUnitType() == UnitTypes::Protoss_Stargate)
			{
				updateStargate(building.first);
			}
			else if (building.second.getUnitType() == UnitTypes::Protoss_Robotics_Facility)
			{
				updateRobo(building.first);
			}

			// Tech Research
			else if (building.second.getUnitType() == UnitTypes::Protoss_Templar_Archives)
			{
				if (!Broodwar->self()->hasResearched(TechTypes::Psionic_Storm))
				{
					if (Broodwar->self()->minerals() >= TechTypes::Psionic_Storm.mineralPrice() && Broodwar->self()->gas() >= TechTypes::Psionic_Storm.gasPrice())
					{
						building.first->research(TechTypes::Psionic_Storm);
						idleTech.erase(building.first);
					}
					else
					{
						idleTech.emplace(building.first, TechTypes::Psionic_Storm);
					}
				}
				else if (Broodwar->self()->getUpgradeLevel(UpgradeTypes::Khaydarin_Amulet) == 0 && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_High_Templar) > 2)
				{
					if (Broodwar->self()->minerals() >= UpgradeTypes::Khaydarin_Amulet.mineralPrice() + queuedMineral && Broodwar->self()->gas() >= UpgradeTypes::Khaydarin_Amulet.gasPrice())
					{
						building.first->upgrade(UpgradeTypes::Khaydarin_Amulet);
						idleUpgrade.erase(building.first);
					}
					else
					{
						idleUpgrade.emplace(building.first, UpgradeTypes::Khaydarin_Amulet);
					}
				}
			}
			else if (building.second.getUnitType() == UnitTypes::Protoss_Arbiter_Tribunal)
			{
				if (!Broodwar->self()->hasResearched(TechTypes::Stasis_Field))
				{
					if (Broodwar->self()->minerals() >= TechTypes::Stasis_Field.mineralPrice() + queuedMineral && Broodwar->self()->gas() >= TechTypes::Stasis_Field.gasPrice() + queuedGas)
					{
						building.first->research(TechTypes::Stasis_Field);
						idleTech.erase(building.first);
					}
					else
					{
						idleTech.emplace(building.first, TechTypes::Stasis_Field);
					}
				}
			}
		}
	}
	return;
}

void ProductionTrackerClass::updateReservedResources()
{
	// Reserved minerals for idle buildings, tech and upgrades
	reservedMineral = 0, reservedGas = 0;
	for (auto b : idleBuildings)
	{
		reservedMineral += b.second.mineralPrice();
		reservedGas += b.second.gasPrice();
	}
	for (auto t : idleTech)
	{
		reservedMineral += t.second.mineralPrice();
		reservedGas += t.second.gasPrice();
	}
	for (auto u : idleUpgrade)
	{
		reservedMineral += u.second.mineralPrice();
		reservedGas += u.second.gasPrice();
	}
	return;
}

void ProductionTrackerClass::updateGateway(Unit building)
{
	int supply = Units().getSupply();
	int queuedMineral = Buildings().getQueuedMineral();
	int queuedGas = Buildings().getQueuedGas();

	// If we need a High Templar
	if (Broodwar->self()->hasResearched(TechTypes::Psionic_Storm) && Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Templar_Archives) >= 1 && Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_High_Templar) < 5)
	{
		// If we can afford a High Temlar, train
		if (Broodwar->self()->minerals() >= UnitTypes::Protoss_High_Templar.mineralPrice() + queuedMineral + reservedMineral && Broodwar->self()->gas() >= UnitTypes::Protoss_High_Templar.gasPrice() + queuedGas + reservedGas && supply + UnitTypes::Protoss_High_Templar.supplyRequired() <= Broodwar->self()->supplyTotal())
		{
			building->train(UnitTypes::Protoss_High_Templar);
			idleGates.erase(building);
			return;
		}
		else
		{
			idleGates.emplace(building, UnitTypes::Protoss_High_Templar);
		}
	}
	// If we need a Dragoon
	if ((noZealots || Strategy().getUnitScore()[UnitTypes::Protoss_Dragoon] >= Strategy().getUnitScore()[UnitTypes::Protoss_Zealot]) && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Cybernetics_Core) > 0)
	{
		// If we can afford a Dragoon, train
		if (Broodwar->self()->minerals() >= UnitTypes::Protoss_Dragoon.mineralPrice() + queuedMineral + reservedMineral && Broodwar->self()->gas() >= UnitTypes::Protoss_Dragoon.gasPrice() + queuedGas + reservedGas && supply + UnitTypes::Protoss_Dragoon.supplyRequired() <= Broodwar->self()->supplyTotal())
		{
			building->train(UnitTypes::Protoss_Dragoon);
			idleGates.erase(building);
			return;
		}
		else
		{
			idleGates.emplace(building, UnitTypes::Protoss_Dragoon);
		}
	}
	// If we need a Zealot
	if (!noZealots && (Strategy().getUnitScore()[UnitTypes::Protoss_Dragoon] < Strategy().getUnitScore()[UnitTypes::Protoss_Zealot] || ((Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Cybernetics_Core) < 1) || Broodwar->self()->gas() < UnitTypes::Protoss_Dragoon.gasPrice() + queuedGas + reservedGas)))
	{
		// If we can afford a Zealot, train
		if (Broodwar->self()->minerals() >= UnitTypes::Protoss_Zealot.mineralPrice() + queuedMineral + reservedMineral && supply + UnitTypes::Protoss_Zealot.supplyRequired() <= Broodwar->self()->supplyTotal())
		{
			building->train(UnitTypes::Protoss_Zealot);
			idleGates.erase(building);
			return;
		}
		else
		{
			idleGates.emplace(building, UnitTypes::Protoss_Zealot);
		}
	}
	return;
}

void ProductionTrackerClass::updateRobo(Unit building)
{
	int supply = Units().getSupply();
	int queuedMineral = Buildings().getQueuedMineral();
	int queuedGas = Buildings().getQueuedGas();

	// If detection is absolutely needed, cancel anything in queue and get the Observer immediately
	if (Strategy().needDetection() && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Observer) == 0)
	{
		if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Observatory) > 0 && building->isTraining())
		{
			for (auto unit : building->getTrainingQueue())
			{
				if (unit == UnitTypes::Protoss_Reaver || unit == UnitTypes::Protoss_Shuttle)
				{
					building->cancelTrain();
				}
			}
		}
		if (Broodwar->self()->minerals() >= UnitTypes::Protoss_Observer.mineralPrice() && Broodwar->self()->gas() >= UnitTypes::Protoss_Observer.gasPrice())
		{
			building->train(UnitTypes::Protoss_Observer);
			idleBuildings.erase(building);
			return;
		}
		else
		{
			idleBuildings.emplace(building, UnitTypes::Protoss_Observer);
		}
	}

	// If we need an Observer
	if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Observatory) > 0 && Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Observer) < (floor(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Reaver) / 3) + 1))
	{
		// If we can afford an Observer, train, otherwise, add to priority
		if (Broodwar->self()->minerals() >= UnitTypes::Protoss_Observer.mineralPrice() + queuedMineral && Broodwar->self()->gas() >= UnitTypes::Protoss_Observer.gasPrice() + queuedGas)
		{
			building->train(UnitTypes::Protoss_Observer);
			idleBuildings.erase(building);
			return;
		}
		else
		{
			idleBuildings.emplace(building, UnitTypes::Protoss_Observer);
		}
	}

	// If we need a Shuttle
	else if ((Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Reaver) / 2 > Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Shuttle)) || (Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Reaver) > 0 && Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Shuttle) <= 0))
	{
		// If we can afford a Shuttle, train, otherwise, add to priority
		if (Broodwar->self()->minerals() >= UnitTypes::Protoss_Shuttle.mineralPrice() + queuedMineral)
		{
			building->train(UnitTypes::Protoss_Shuttle);
			idleBuildings.erase(building);
		}
		else
		{
			idleBuildings.emplace(building, UnitTypes::Protoss_Shuttle);
		}
	}

	// If we need a Reaver			
	else if (Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Robotics_Support_Bay) > 0 && Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Reaver) < 10)
	{
		// If we can afford a Reaver, train, otherwise, add to priority
		if (Broodwar->self()->minerals() >= UnitTypes::Protoss_Reaver.mineralPrice() + queuedMineral && Broodwar->self()->gas() >= UnitTypes::Protoss_Reaver.gasPrice() + queuedGas)
		{
			building->train(UnitTypes::Protoss_Reaver);
			idleBuildings.erase(building);
			return;
		}
		else
		{
			idleBuildings.emplace(building, UnitTypes::Protoss_Reaver);
		}
	}
}

void ProductionTrackerClass::updateStargate(Unit building)
{
	int supply = Units().getSupply();
	int queuedMineral = Buildings().getQueuedMineral();
	int queuedGas = Buildings().getQueuedGas();

	// Set as visible so it saves resources for Arbiters if we're teching to them
	if (Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Arbiter_Tribunal) > 0 && Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Arbiter) < 3)
	{
		if (Broodwar->self()->minerals() >= UnitTypes::Protoss_Arbiter.mineralPrice() + queuedMineral && Broodwar->self()->gas() >= UnitTypes::Protoss_Arbiter.gasPrice() + queuedGas)
		{
			building->train(UnitTypes::Protoss_Arbiter);
			idleBuildings.erase(building);
			return;
		}
		else
		{
			idleBuildings.emplace(building, UnitTypes::Protoss_Arbiter);
		}
	}
	// Only build corsairs against Zerg
	if (Broodwar->enemy()->getRace() == Races::Zerg && Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Corsair) < 10)
	{
		if (Broodwar->self()->minerals() >= UnitTypes::Protoss_Corsair.mineralPrice() + queuedMineral && Broodwar->self()->gas() >= UnitTypes::Protoss_Corsair.gasPrice() + queuedGas)
		{
			building->train(UnitTypes::Protoss_Corsair);
		}
	}
}

void ProductionTrackerClass::updateLuxuryTech(Unit building)
{

}