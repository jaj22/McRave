#include "ProductionManager.h"
#include "GridManager.h"
#include "UnitManager.h"
#include "StrategyManager.h"
#include "BuildingManager.h"
#include "BuildOrder.h"

void ProductionTrackerClass::update()
{
	updateReservedResources();
	int supply = UnitTracker::Instance().getSupply();
	int queuedMineral = BuildingTracker::Instance().getQueuedMineral();
	int queuedGas = BuildingTracker::Instance().getQueuedGas();
	if (Broodwar->enemy()->getRace() == Races::Terran)
	{
		noZealots = true;
	}
	if (Broodwar->self()->completedUnitCount(Protoss_Nexus) >= 2)
	{
		noZealots = false;
	}

	for (auto building : BuildingTracker::Instance().getMyBuildings())
	{
		if (building.first->isIdle())
		{
			if (building.second.getType() == Protoss_Forge)
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
			else if (building.second.getType() == Protoss_Cybernetics_Core)
			{
				if (!Broodwar->self()->getUpgradeLevel(UpgradeTypes::Singularity_Charge) && idleGates.size() == 0 && Broodwar->self()->visibleUnitCount(Protoss_Dragoon) >= 3)
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
			else if (building.second.getType() == Protoss_Robotics_Support_Bay)
			{
				if (Broodwar->self()->completedUnitCount(Protoss_Reaver) > 2)
				{
					if (Broodwar->enemy()->getRace() != Races::Zerg && Broodwar->self()->minerals() >= UpgradeTypes::Scarab_Damage.mineralPrice() + queuedMineral + reservedMineral && Broodwar->self()->gas() >= UpgradeTypes::Scarab_Damage.gasPrice() + queuedGas + reservedGas)
					{
						building.first->upgrade(UpgradeTypes::Scarab_Damage);
					}
				}
				if (Broodwar->self()->completedUnitCount(Protoss_Shuttle) >= 2)
				{
					// Shuttle speed
				}
			}
			else if (building.second.getType() == Protoss_Fleet_Beacon)
			{
				if (Broodwar->self()->visibleUnitCount(Protoss_Carrier) > 2)
				{
					if (Broodwar->self()->minerals() >= UpgradeTypes::Carrier_Capacity.mineralPrice() + queuedMineral + reservedMineral && Broodwar->self()->gas() >= UpgradeTypes::Carrier_Capacity.gasPrice() + queuedGas + reservedGas)
					{
						building.first->upgrade(UpgradeTypes::Carrier_Capacity);
					}
				}
			}
			else if (building.second.getType() == Protoss_Citadel_of_Adun)
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
			else if (building.second.getType() == Protoss_Gateway)
			{
				updateGateway(building.first);
			}
			else if (building.second.getType() == Protoss_Stargate)
			{
				updateStargate(building.first);
			}
			else if (building.second.getType() == Protoss_Robotics_Facility)
			{
				updateRobo(building.first);
			}

			// Tech Research
			else if (building.second.getType() == Protoss_Templar_Archives)
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
				else if (Broodwar->self()->getUpgradeLevel(UpgradeTypes::Khaydarin_Amulet) == 0 && Broodwar->self()->completedUnitCount(Protoss_High_Templar) > 2)
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
			else if (building.second.getType() == Protoss_Arbiter_Tribunal)
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
	int supply = UnitTracker::Instance().getSupply();
	int queuedMineral = BuildingTracker::Instance().getQueuedMineral();
	int queuedGas = BuildingTracker::Instance().getQueuedGas();
	// If we need a High Templar
	if (Broodwar->self()->hasResearched(TechTypes::Psionic_Storm) && Broodwar->self()->visibleUnitCount(Protoss_Templar_Archives) >= 1 && Broodwar->self()->visibleUnitCount(Protoss_High_Templar) < 5)
	{
		// If we can afford a High Temlar, train
		if (Broodwar->self()->minerals() >= Protoss_High_Templar.mineralPrice() + queuedMineral + reservedMineral && Broodwar->self()->gas() >= Protoss_High_Templar.gasPrice() + queuedGas + reservedGas && supply + Protoss_High_Templar.supplyRequired() <= Broodwar->self()->supplyTotal())
		{
			building->train(Protoss_High_Templar);
			idleGates.erase(building);
			return;
		}
		else
		{
			idleGates.emplace(building, Protoss_High_Templar);
		}
	}
	// If we need a Dragoon
	if ((noZealots || StrategyTracker::Instance().getUnitScore()[Protoss_Dragoon] >= StrategyTracker::Instance().getUnitScore()[Protoss_Zealot]) && Broodwar->self()->completedUnitCount(Protoss_Cybernetics_Core) > 0)
	{
		// If we can afford a Dragoon, train
		if (Broodwar->self()->minerals() >= Protoss_Dragoon.mineralPrice() + queuedMineral + reservedMineral && Broodwar->self()->gas() >= Protoss_Dragoon.gasPrice() + queuedGas + reservedGas && supply + Protoss_Dragoon.supplyRequired() <= Broodwar->self()->supplyTotal())
		{
			building->train(Protoss_Dragoon);
			idleGates.erase(building);
			return;
		}
		else
		{
			idleGates.emplace(building, Protoss_Dragoon);
		}
	}
	// If we need a Zealot
	if (!noZealots && (StrategyTracker::Instance().getUnitScore()[Protoss_Dragoon] < StrategyTracker::Instance().getUnitScore()[Protoss_Zealot] || ((Broodwar->self()->completedUnitCount(Protoss_Cybernetics_Core) < 1) || Broodwar->self()->gas() < Protoss_Dragoon.gasPrice() + queuedGas + reservedGas)))
	{
		// If we can afford a Zealot, train
		if (Broodwar->self()->minerals() >= Protoss_Zealot.mineralPrice() + queuedMineral + reservedMineral && supply + Protoss_Zealot.supplyRequired() <= Broodwar->self()->supplyTotal())
		{
			building->train(Protoss_Zealot);
			idleGates.erase(building);
			return;
		}
		else
		{
			idleGates.emplace(building, Protoss_Zealot);
		}
	}
	return;
}

void ProductionTrackerClass::updateRobo(Unit building)
{
	int supply = UnitTracker::Instance().getSupply();
	int queuedMineral = BuildingTracker::Instance().getQueuedMineral();
	int queuedGas = BuildingTracker::Instance().getQueuedGas();

	// If detection is absolutely needed, cancel anything in queue and get the Observer immediately
	if (StrategyTracker::Instance().needDetection() && Broodwar->self()->completedUnitCount(Protoss_Observer) == 0)
	{
		if (Broodwar->self()->completedUnitCount(Protoss_Observatory) > 0 && building->isTraining())
		{
			for (auto unit : building->getTrainingQueue())
			{
				if (unit == Protoss_Reaver || unit == Protoss_Shuttle)
				{
					building->cancelTrain();
				}
			}
		}
		if (Broodwar->self()->minerals() >= Protoss_Observer.mineralPrice() && Broodwar->self()->gas() >= Protoss_Observer.gasPrice())
		{
			building->train(Protoss_Observer);
			idleBuildings.erase(building);
			return;
		}
		else
		{
			idleBuildings.emplace(building, Protoss_Observer);
		}
	}

	// If we need an Observer
	if (Broodwar->self()->completedUnitCount(Protoss_Observatory) > 0 && Broodwar->self()->visibleUnitCount(Protoss_Observer) < (floor(Broodwar->self()->visibleUnitCount(Protoss_Reaver) / 3) + 1))
	{
		// If we can afford an Observer, train, otherwise, add to priority
		if (Broodwar->self()->minerals() >= Protoss_Observer.mineralPrice() + queuedMineral && Broodwar->self()->gas() >= Protoss_Observer.gasPrice() + queuedGas)
		{
			building->train(Protoss_Observer);
			idleBuildings.erase(building);
			return;
		}
		else
		{
			idleBuildings.emplace(building, Protoss_Observer);
		}
	}

	// If we need a Shuttle
	else if ((Broodwar->self()->visibleUnitCount(Protoss_Reaver) / 2 > Broodwar->self()->visibleUnitCount(Protoss_Shuttle)) || (Broodwar->self()->visibleUnitCount(Protoss_Reaver) > 0 && Broodwar->self()->visibleUnitCount(Protoss_Shuttle) <= 0))
	{
		// If we can afford a Shuttle, train, otherwise, add to priority
		if (Broodwar->self()->minerals() >= Protoss_Shuttle.mineralPrice() + queuedMineral)
		{
			building->train(Protoss_Shuttle);
			idleBuildings.erase(building);
		}
		else
		{
			idleBuildings.emplace(building, Protoss_Shuttle);
		}
	}

	// If we need a Reaver			
	else if (Broodwar->self()->completedUnitCount(Protoss_Robotics_Support_Bay) > 0 && Broodwar->self()->visibleUnitCount(Protoss_Reaver) < 10)
	{
		// If we can afford a Reaver, train, otherwise, add to priority
		if (Broodwar->self()->minerals() >= Protoss_Reaver.mineralPrice() + queuedMineral && Broodwar->self()->gas() >= Protoss_Reaver.gasPrice() + queuedGas)
		{
			building->train(Protoss_Reaver);
			idleBuildings.erase(building);
			return;
		}
		else
		{
			idleBuildings.emplace(building, Protoss_Reaver);
		}
	}
}

void ProductionTrackerClass::updateStargate(Unit building)
{
	int supply = UnitTracker::Instance().getSupply();
	int queuedMineral = BuildingTracker::Instance().getQueuedMineral();
	int queuedGas = BuildingTracker::Instance().getQueuedGas();

	// Set as visible so it saves resources for Arbiters if we're teching to them
	if (Broodwar->self()->visibleUnitCount(Protoss_Arbiter_Tribunal) > 0 && Broodwar->self()->visibleUnitCount(Protoss_Arbiter) < 3)
	{
		if (Broodwar->self()->minerals() >= Protoss_Arbiter.mineralPrice() + queuedMineral && Broodwar->self()->gas() >= Protoss_Arbiter.gasPrice() + queuedGas)
		{
			building->train(Protoss_Arbiter);
			idleBuildings.erase(building);
			return;
		}
		else
		{
			idleBuildings.emplace(building, Protoss_Arbiter);
		}
	}
	// Only build corsairs against Zerg
	if (Broodwar->enemy()->getRace() == Races::Zerg && Broodwar->self()->visibleUnitCount(Protoss_Corsair) < 10)
	{
		if (Broodwar->self()->minerals() >= Protoss_Corsair.mineralPrice() + queuedMineral && Broodwar->self()->gas() >= Protoss_Corsair.gasPrice() + queuedGas)
		{
			building->train(Protoss_Corsair);
		}
	}
}

void ProductionTrackerClass::updateLuxuryTech(Unit building)
{

}