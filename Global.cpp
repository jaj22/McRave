// CMProtoBot is made by Christian McCrave
// Twitch nickname McRave \o/
// For any questions, email christianmccrave@gmail.com
// Bot started 01/03/2017

// Include header with all includes in them
#include "Header.h"
#include "Global.h"

// Namespaces
using namespace std;
using namespace BWAPI;
using namespace BWTA;
using namespace Filter;
using namespace BWEM;
namespace { auto & theMap = BWEM::Map::Instance(); }

bool analyzed;
bool analysis_just_finished;

void McRave::onStart()
{
	// Enable the UserInput flag, which allows us to control the bot and type messages.
	Broodwar->enableFlag(Flag::UserInput);

	// Set the command optimization level so that common commands can be grouped and reduce the bot's APM (Actions Per Minute).
	Broodwar->setCommandOptimizationLevel(2);

	if (Broodwar->enemy())
	{
		Broodwar << "The matchup is " << Broodwar->self()->getRace() << " vs " << Broodwar->enemy()->getRace() << endl;
	}

	theMap.Initialize();
	theMap.EnableAutomaticPathAnalysis();
	bool startingLocationsOK = theMap.FindBasesForStartingLocations();
	assert(startingLocationsOK);

	BWEM::utils::MapPrinter::Initialize(&theMap);
	BWEM::utils::printMap(theMap);      // will print the map into the file <StarCraftFolder>bwapi-data/map.bmp
	BWEM::utils::pathExample(theMap);   // add to the printed map a path between two starting locations

	if (analyzed == false) {
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AnalyzeThread, NULL, 0, NULL);
	}
	readMap();
	analyzed = false;
	analysis_just_finished = false;
}

void McRave::onEnd(bool isWinner)
{
	// Called when the game ends
	if (isWinner)
	{
		// Log your win here!
	}
}

void McRave::onFrame()
{
#pragma region Heatmap Manager
	{
		double strongest = 0;
		// For each tile, draw the current threat onto the tile
		for (int x = 0; x <= Broodwar->mapWidth(); x++)
		{
			for (int y = 0; y <= Broodwar->mapHeight(); y++)
			{
				
				if (enemyHeatmap[x][y] > 0)
				{					
					//Broodwar->drawTextMap(x * 32, y * 32, "%.2f", enemyHeatmap[x][y]);
				}
				if (shuttleHeatmap[x][y] > 0)
				{			
					//Broodwar->drawTextMap(x * 32, y * 32, "%d", shuttleHeatmap[x][y]);
				}
				if (allyHeatmap[x][y] > 0)
				{
					//Broodwar->drawTextMap(x * 32, y * 32, "%.2f", allyHeatmap[x][y]);
				}
				
				if (allyHeatmap[x][y] - enemyHeatmap[x][y] > strongest && allyHeatmap[x][y] > 0)
				{
					arbiterPosition = Position(x * 32, y * 32);
					strongest = allyHeatmap[x][y] - enemyHeatmap[x][y];
				}

				if (Broodwar->isVisible(x, y))
				{
					enemyHeatmap[x][y] = 0;
					allyHeatmap[x][y] = 0;
					airEnemyHeatmap[x][y] = 0;
				}
			}
		}
		// For each enemy unit, add its attack value to each tile it is in range of
		for (auto u : Broodwar->enemy()->getUnits())
		{
			if (u->getType().groundWeapon().damageAmount() > 0)
			{
				int range;
				// Making sure we properly analyze the threat of melee units without adding range to ranged units
				if (u->getType().groundWeapon().maxRange() < 32)
				{
					range = (u->getType().groundWeapon().maxRange() + 64) / 32;
				}
				else
				{
					range = (u->getType().groundWeapon().maxRange()) / 32;
				}
				// The + 1 is because we need to still check an additional tile
				for (int x = u->getTilePosition().x - range; x <= u->getTilePosition().x + range + 1; x++)
				{
					for (int y = u->getTilePosition().y - range; y <= u->getTilePosition().y + range + 1; y++)
					{
						if (Broodwar->isVisible(x, y) && (u->getDistance(Position((x * 32), (y * 32))) <= (range * 32)) && (x > 0 || x <= Broodwar->mapWidth() || y > 0 || y <= Broodwar->mapHeight()))
						{
							enemyHeatmap[x][y] += unitGetStrength(u);
						}
					}
				}
			}
			if (u->getType().airWeapon().damageAmount() > 0)
			{
				int range = (u->getType().airWeapon().maxRange()) / 32;
				// The + 1 is because we need to still check an additional tile
				for (int x = u->getTilePosition().x - range; x <= u->getTilePosition().x + range + 1; x++)
				{
					for (int y = u->getTilePosition().y - range; y <= u->getTilePosition().y + range + 1; y++)
					{
						if (Broodwar->isVisible(x, y) && (u->getDistance(Position((x * 32), (y * 32))) <= (range * 32)) && (x > 0 || x <= Broodwar->mapWidth() || y > 0 || y <= Broodwar->mapHeight()))
						{
							airEnemyHeatmap[x][y] += unitGetStrength(u);
						}
					}
				}
			}
		}
		for (auto u : Broodwar->self()->getUnits())
		{
			if (u->getType().groundWeapon().damageAmount() > 0)
			{
				int range;
				// Making sure we properly analyze the threat of melee units without adding range to ranged units
				if (u->getType().groundWeapon().maxRange() < 32)
				{
					range = (u->getType().groundWeapon().maxRange() + 64) / 32;
				}
				else
				{
					range = (u->getType().groundWeapon().maxRange()) / 32;
				}
				// The + 1 is because we need to still check an additional tile
				for (int x = u->getTilePosition().x - range; x <= u->getTilePosition().x + range + 1; x++)
				{
					for (int y = u->getTilePosition().y - range; y <= u->getTilePosition().y + range + 1; y++)
					{
						if (Broodwar->isVisible(x, y) && (u->getDistance(Position((x * 32), (y * 32))) <= (range * 32)) && (x > 0 || x <= Broodwar->mapWidth() || y > 0 || y <= Broodwar->mapHeight()))
						{
							allyHeatmap[x][y] += unitGetStrength(u);
						}
					}
				}
			}
		}
	}
#pragma endregion
#pragma region Territory Manager
	{
		if (BWTAhandling)
		{
			if (territory.size() < 1)
			{
				allyTerritory.push_back(BWTA::getRegion(playerStartingPosition));
				territory = BWTA::getRegions();
			}
			// In each ally territory there are chokepoints to defend, find and store those chokepoints
			if (allyTerritory.size() != currentSize && allyTerritory.size() > 0)
			{
				// For all regions
				for (auto *region : territory)
				{
					bool merge = false;
					// For each chokepoint
					for (auto Chokepoint : region->getChokepoints())
					{
						// Check if every chokepoint is connected to an ally territory, in which case add this to our territory (connecting regions)
						if (find(allyTerritory.begin(), allyTerritory.end(), Chokepoint->getRegions().first) != allyTerritory.end() || find(allyTerritory.begin(), allyTerritory.end(), Chokepoint->getRegions().second) != allyTerritory.end())
						{
							merge = true;
						}
						else
						{
							merge = false;
							break;
						}
					}
					if (merge == true)
					{
						allyTerritory.push_back(region);
					}
				}
				defendHere.erase(defendHere.begin(), defendHere.end());
				currentSize = allyTerritory.size();
				// For each region that is ally territory
				for (auto *region : allyTerritory)
				{
					// For each chokepoint of each ally region				
					for (auto Chokepoint : region->getChokepoints())
					{
						// Check if both territories are ally
						if (find(allyTerritory.begin(), allyTerritory.end(), Chokepoint->getRegions().first) != allyTerritory.end() && find(allyTerritory.begin(), allyTerritory.end(), Chokepoint->getRegions().second) != allyTerritory.end())
						{
							// If both are ally, do nothing (we don't need to defend two ally regions
						}
						else
						{
							defendHere.push_back(Chokepoint->getCenter());
						}
					}
				}
			}
			// Draw territory boundaries
			for (auto position : defendHere)
			{
				Broodwar->drawCircleMap(position, 80, Colors::Green);
			}
		}
	}
#pragma endregion
#pragma region Base Manager
	{
		// BWTA draw
		if (analyzed)
		{

			drawTerrainData();
			if (doOnce)
			{
				buildChokeNodes();
				doOnce = false;

				// Find player starting position and tile position		
				BaseLocation* playerStartingLocation = getStartLocation(Broodwar->self());
				playerStartingPosition = playerStartingLocation->getPosition();
				playerStartingTilePosition = playerStartingLocation->getTilePosition();		
				nextExpansion.push_back(playerStartingTilePosition);
				activeExpansion.push_back(playerStartingTilePosition);				
				BWTAhandling = true;
			}
		}
		// Only do this loop once if map analysis done
		if (analysis_just_finished)
		{
			if (enemyBasePositions.size() > 0)
			{
				analysis_just_finished = false;
				set<BaseLocation*> allBases = getBaseLocations();
				for (auto base : allBases)
				{
					int distances = getGroundDistance2(base->getTilePosition(), playerStartingTilePosition) - getGroundDistance2(base->getTilePosition(), enemyStartingTilePosition);
					if (base->isMineralOnly())
					{
						distances += 1280;
					}
					if (base->getTilePosition() != playerStartingTilePosition)
					{
						testBases.emplace(distances, base->getTilePosition());
					}					
				}

				for (auto base : testBases)
				{
					nextExpansion.push_back(base.second);
					activeExpansion.push_back(base.second);
				}							
			}
		}

		if (BWTAhandling)
		{
			for (int i = 0; i <= (int)activeExpansion.size() - 1; i++)
			{
				Broodwar->drawTextMap(activeExpansion.at(i).x * 32, activeExpansion.at(i).y * 32, "Base %d", i, Colors::White);
			}
			// Get build order based on enemy race(s)
			getBuildOrder();
		}
	}
#pragma endregion
#pragma region HUD
	{
		// Display some information about our buildings
		Broodwar->drawTextScreen(0, 0, "Building Count/Desired");
		Broodwar->drawTextScreen(0, 10, "Nexus:");
		Broodwar->drawTextScreen(0, 20, "Pylon:");
		Broodwar->drawTextScreen(0, 30, "Gas:");
		Broodwar->drawTextScreen(0, 40, "Gate:");
		Broodwar->drawTextScreen(0, 50, "Forge:");
		Broodwar->drawTextScreen(0, 60, "Core:");
		Broodwar->drawTextScreen(0, 70, "RoboF:");
		Broodwar->drawTextScreen(0, 80, "Stargate:");
		Broodwar->drawTextScreen(0, 90, "Citadel:");
		Broodwar->drawTextScreen(0, 100, "Support:");
		Broodwar->drawTextScreen(0, 110, "Fleet:");
		Broodwar->drawTextScreen(0, 120, "Archives:");
		Broodwar->drawTextScreen(0, 130, "Observe:");
		Broodwar->drawTextScreen(0, 140, "Tribunal:");

		// Counters
		Broodwar->drawTextScreen(50, 10, "%d  %d Inactive: %d", Broodwar->self()->allUnitCount(UnitTypes::Protoss_Nexus), nexusDesired, inactiveNexusCnt);
		Broodwar->drawTextScreen(50, 20, "%d  %d", Broodwar->self()->allUnitCount(UnitTypes::Protoss_Pylon), pylonDesired);
		Broodwar->drawTextScreen(50, 30, "%d  %d", Broodwar->self()->allUnitCount(UnitTypes::Protoss_Assimilator), gasDesired);
		Broodwar->drawTextScreen(50, 40, "%d  %d", Broodwar->self()->allUnitCount(UnitTypes::Protoss_Gateway), gateDesired);
		Broodwar->drawTextScreen(50, 50, "%d  %d", Broodwar->self()->allUnitCount(UnitTypes::Protoss_Forge), forgeDesired);
		Broodwar->drawTextScreen(50, 60, "%d  %d", Broodwar->self()->allUnitCount(UnitTypes::Protoss_Cybernetics_Core), coreDesired);
		Broodwar->drawTextScreen(50, 70, "%d  %d", Broodwar->self()->allUnitCount(UnitTypes::Protoss_Robotics_Facility), roboDesired);
		Broodwar->drawTextScreen(50, 80, "%d  %d", Broodwar->self()->allUnitCount(UnitTypes::Protoss_Stargate), stargateDesired);
		Broodwar->drawTextScreen(50, 90, "%d  %d", Broodwar->self()->allUnitCount(UnitTypes::Protoss_Citadel_of_Adun), citadelDesired);
		Broodwar->drawTextScreen(50, 100, "%d  %d", Broodwar->self()->allUnitCount(UnitTypes::Protoss_Robotics_Support_Bay), supportBayDesired);
		Broodwar->drawTextScreen(50, 110, "%d  %d", Broodwar->self()->allUnitCount(UnitTypes::Protoss_Fleet_Beacon), fleetBeaconDesired);
		Broodwar->drawTextScreen(50, 120, "%d  %d", Broodwar->self()->allUnitCount(UnitTypes::Protoss_Templar_Archives), archivesDesired);
		Broodwar->drawTextScreen(50, 130, "%d  %d", Broodwar->self()->allUnitCount(UnitTypes::Protoss_Observatory), observatoryDesired);
		Broodwar->drawTextScreen(50, 140, "%d  %d", Broodwar->self()->allUnitCount(UnitTypes::Protoss_Arbiter_Tribunal), tribunalDesired);

		// Display some information about our queued resources required for structure building
		Broodwar->drawTextScreen(200, 0, "Queued Minerals: %d", queuedMineral);
		Broodwar->drawTextScreen(200, 10, "Queued Gas: %d", queuedGas);
		Broodwar->drawTextScreen(200, 20, "Reserved Minerals: %d", reservedMineral);
		Broodwar->drawTextScreen(200, 30, "Reserved Gas: %d", reservedGas);
		Broodwar->drawTextScreen(200, 40, "%d", Broodwar->getFrameCount());

		// Display some information about our units
		Broodwar->drawTextScreen(500, 20, "Unit Count");
		Broodwar->drawTextScreen(500, 30, "Scouting: %d", scouting);
		Broodwar->drawTextScreen(500, 40, "Ally Supply: %d", allySupply);
		Broodwar->drawTextScreen(500, 50, "Enemy Supply: %d", enemySupply);
		Broodwar->drawTextScreen(500, 60, "Ally Strength: %.2f", allyStrength);
		Broodwar->drawTextScreen(500, 70, "Enemy Strength: %.2f", enemyStrength);
		Broodwar->drawTextScreen(500, 80, "Force Engage: %d", forceEngage);

		// Display which worker is builder and scouter
		if (Broodwar->getFrameCount() > 100)
		{
			Broodwar->drawTextMap(Broodwar->getUnit(buildingWorkerID.front())->getPosition(), "Builder", Colors::Yellow);
			Broodwar->drawTextMap(Broodwar->getUnit(scoutWorkerID.front())->getPosition(), "Scouter", Colors::Yellow);
		}
		
		// Arbiter Position showing
		//Broodwar->drawCircleMap(arbiterPosition, 8, Colors::Purple, true);

		// Show building placement
		TilePosition expectedRightCorner;
		expectedRightCorner.x = buildTilePosition.x + currentBuilding.tileWidth();
		expectedRightCorner.y = buildTilePosition.y + currentBuilding.tileHeight();
		Broodwar->drawBoxMap(buildTilePosition.x * 32, buildTilePosition.y * 32, expectedRightCorner.x * 32, expectedRightCorner.y * 32, Colors::Black, false);
	}
#pragma endregion
#pragma region Scouting Midgame
	{
		// Scouting if we can't find enemy bases
		if (enemyBasePositions.size() < 1 && Broodwar->getFrameCount() > 10000 && Broodwar->getFrameCount() > enemyScoutedLast + 1000 && Broodwar->getUnitsInRadius(playerStartingPosition, 50000, Filter::IsBuilding && Filter::IsEnemy).size() < 1)
		{
			enemyScoutedLast = Broodwar->getFrameCount();
			Broodwar << "Can't find enemy, scouting out." << endl;
			for (auto base : getBaseLocations())
			{
				if (!Broodwar->isVisible(base->getTilePosition()))
				{
					Broodwar->getClosestUnit(base->getPosition(), Filter::IsAlly && !Filter::IsBuilding && !Filter::IsWorker)->attack(base->getPosition());
				}
			}
		}
	}
#pragma endregion
#pragma region Structure Information	
	{
		// If a structure is required, make sure we notify production that we have an additional cost to not eat into
		queuedMineral = std::max(0, (nexusDesired + inactiveNexusCnt - Broodwar->self()->allUnitCount(UnitTypes::Protoss_Nexus))*(UnitTypes::Protoss_Nexus.mineralPrice()))
			+ std::max(0, (pylonDesired - Broodwar->self()->allUnitCount(UnitTypes::Protoss_Pylon))*(UnitTypes::Protoss_Pylon.mineralPrice()))
			+ std::max(0, (gasDesired - Broodwar->self()->allUnitCount(UnitTypes::Protoss_Assimilator))*(UnitTypes::Protoss_Assimilator.mineralPrice()))
			+ std::max(0, (gateDesired - Broodwar->self()->allUnitCount(UnitTypes::Protoss_Gateway))*(UnitTypes::Protoss_Gateway.mineralPrice()))
			+ std::max(0, (forgeDesired - Broodwar->self()->allUnitCount(UnitTypes::Protoss_Forge))*(UnitTypes::Protoss_Forge.mineralPrice()))
			+ std::max(0, (batteryDesired - Broodwar->self()->allUnitCount(UnitTypes::Protoss_Shield_Battery))*(UnitTypes::Protoss_Shield_Battery.mineralPrice()))
			+ std::max(0, (coreDesired - Broodwar->self()->allUnitCount(UnitTypes::Protoss_Cybernetics_Core))*(UnitTypes::Protoss_Cybernetics_Core.mineralPrice()))
			+ std::max(0, (roboDesired - Broodwar->self()->allUnitCount(UnitTypes::Protoss_Robotics_Facility))*(UnitTypes::Protoss_Robotics_Facility.mineralPrice()))
			+ std::max(0, (stargateDesired - Broodwar->self()->allUnitCount(UnitTypes::Protoss_Stargate))*(UnitTypes::Protoss_Stargate.mineralPrice()))
			+ std::max(0, (citadelDesired - Broodwar->self()->allUnitCount(UnitTypes::Protoss_Citadel_of_Adun))*(UnitTypes::Protoss_Citadel_of_Adun.mineralPrice()))
			+ std::max(0, (supportBayDesired - Broodwar->self()->allUnitCount(UnitTypes::Protoss_Robotics_Support_Bay))*(UnitTypes::Protoss_Robotics_Support_Bay.mineralPrice()))
			+ std::max(0, (fleetBeaconDesired - Broodwar->self()->allUnitCount(UnitTypes::Protoss_Fleet_Beacon))*(UnitTypes::Protoss_Fleet_Beacon.mineralPrice()))
			+ std::max(0, (archivesDesired - Broodwar->self()->allUnitCount(UnitTypes::Protoss_Templar_Archives))*(UnitTypes::Protoss_Templar_Archives.mineralPrice()))
			+ std::max(0, (observatoryDesired - Broodwar->self()->allUnitCount(UnitTypes::Protoss_Observatory))*(UnitTypes::Protoss_Observatory.mineralPrice()))
			+ std::max(0, (tribunalDesired - Broodwar->self()->allUnitCount(UnitTypes::Protoss_Arbiter_Tribunal))*(UnitTypes::Protoss_Arbiter_Tribunal.mineralPrice()))
			;

		queuedGas = std::max(0, (roboDesired - Broodwar->self()->allUnitCount(UnitTypes::Protoss_Robotics_Facility))*(UnitTypes::Protoss_Robotics_Facility.gasPrice()))
			+ std::max(0, (stargateDesired - Broodwar->self()->allUnitCount(UnitTypes::Protoss_Stargate))*(UnitTypes::Protoss_Stargate.gasPrice()))
			+ std::max(0, (citadelDesired - Broodwar->self()->allUnitCount(UnitTypes::Protoss_Citadel_of_Adun))*(UnitTypes::Protoss_Citadel_of_Adun.gasPrice()))
			+ std::max(0, (supportBayDesired - Broodwar->self()->allUnitCount(UnitTypes::Protoss_Robotics_Support_Bay))*(UnitTypes::Protoss_Robotics_Support_Bay.gasPrice()))
			+ std::max(0, (fleetBeaconDesired - Broodwar->self()->allUnitCount(UnitTypes::Protoss_Fleet_Beacon))*(UnitTypes::Protoss_Fleet_Beacon.gasPrice()))
			+ std::max(0, (archivesDesired - Broodwar->self()->allUnitCount(UnitTypes::Protoss_Templar_Archives))*(UnitTypes::Protoss_Templar_Archives.gasPrice()))
			+ std::max(0, (observatoryDesired - Broodwar->self()->allUnitCount(UnitTypes::Protoss_Observatory))*(UnitTypes::Protoss_Observatory.gasPrice()))
			+ std::max(0, (tribunalDesired - Broodwar->self()->allUnitCount(UnitTypes::Protoss_Arbiter_Tribunal))*(UnitTypes::Protoss_Arbiter_Tribunal.gasPrice()))
			;

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
	}
#pragma endregion
#pragma region UnitManager
	{
		// Check all units for their current health, shields and damage capabilities to compare against enemy
		allyStrength = 0.0;
		for (auto u : Broodwar->self()->getUnits())
		{
			allyStrength += unitGetStrength(u);
		}

		// Prevent spamming by only running our onFrame once every number of latency frames.
		// Latency frames are the number of frames before commands are processed.
		if (Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0)
			return;

		// Iterate through all the units that we own
		for (auto u : Broodwar->self()->getUnits())
		{			
			// Ignore the unit if it no longer exists, is locked down, maelstrommed, stassised, loaded, not powered, stuck, not completed
			if (!u->exists() || u->isLockedDown() || u->isMaelstrommed() || u->isStasised()
				|| u->isLoaded() || !u->isPowered() || !u->isCompleted() || u->isStuck())
				continue;
#pragma region Probe Manager
			{
				if (u->getType() == UnitTypes::Protoss_Probe)
				{
					// If no builder exists, this probe is now the builder
					if (find(scoutWorkerID.begin(), scoutWorkerID.end(), u->getID()) == scoutWorkerID.end() && buildingWorkerID.size() < 1)
					{
						buildingWorkerID.push_back(u->getID());
					}
					// Else if no scouter exists, this probe is now the scouter
					else if (find(buildingWorkerID.begin(), buildingWorkerID.end(), u->getID()) == buildingWorkerID.end() && scoutWorkerID.size() < 1)
					{
						scoutWorkerID.push_back(u->getID());
					}
					// Builder commands
					if (u->getID() == buildingWorkerID.front() && (!u->isConstructing() || !u->isMoving()))
					{
						if (Broodwar->self()->allUnitCount(UnitTypes::Protoss_Pylon) < pylonDesired && Broodwar->self()->minerals() >= UnitTypes::Protoss_Pylon.mineralPrice())
						{
							buildingManager(UnitTypes::Protoss_Pylon, u);
						}
						else if (Broodwar->self()->allUnitCount(UnitTypes::Protoss_Assimilator) < gasDesired && Broodwar->self()->minerals() >= UnitTypes::Protoss_Assimilator.mineralPrice())
						{
							u->build(UnitTypes::Protoss_Assimilator, gasTilePosition.back());
						}
						else if (Broodwar->self()->allUnitCount(UnitTypes::Protoss_Gateway) < gateDesired && Broodwar->self()->minerals() >= UnitTypes::Protoss_Gateway.mineralPrice())
						{
							buildingManager(UnitTypes::Protoss_Gateway, u);
						}
						else if (Broodwar->self()->allUnitCount(UnitTypes::Protoss_Forge) < forgeDesired && Broodwar->self()->minerals() >= UnitTypes::Protoss_Forge.mineralPrice())
						{
							buildingManager(UnitTypes::Protoss_Forge, u);
						}
						else if (Broodwar->self()->allUnitCount(UnitTypes::Protoss_Shield_Battery) < batteryDesired && Broodwar->self()->minerals() >= UnitTypes::Protoss_Shield_Battery.mineralPrice())
						{
							buildingManager(UnitTypes::Protoss_Shield_Battery, u);
						}
						else if (Broodwar->self()->allUnitCount(UnitTypes::Protoss_Cybernetics_Core) < coreDesired && Broodwar->self()->minerals() >= UnitTypes::Protoss_Cybernetics_Core.mineralPrice())
						{
							buildingManager(UnitTypes::Protoss_Cybernetics_Core, u);
						}
						else if (Broodwar->self()->allUnitCount(UnitTypes::Protoss_Robotics_Facility) < roboDesired && Broodwar->self()->minerals() >= UnitTypes::Protoss_Robotics_Facility.mineralPrice() && Broodwar->self()->gas() >= UnitTypes::Protoss_Robotics_Facility.gasPrice())
						{
							buildingManager(UnitTypes::Protoss_Robotics_Facility, u);
						}
						else if (Broodwar->self()->allUnitCount(UnitTypes::Protoss_Stargate) < stargateDesired && Broodwar->self()->minerals() >= UnitTypes::Protoss_Stargate.mineralPrice() && Broodwar->self()->gas() >= UnitTypes::Protoss_Stargate.gasPrice())
						{
							buildingManager(UnitTypes::Protoss_Stargate, u);
						}
						else if (Broodwar->self()->allUnitCount(UnitTypes::Protoss_Citadel_of_Adun) < citadelDesired && Broodwar->self()->minerals() >= UnitTypes::Protoss_Citadel_of_Adun.mineralPrice() && Broodwar->self()->gas() >= UnitTypes::Protoss_Citadel_of_Adun.gasPrice())
						{
							buildingManager(UnitTypes::Protoss_Citadel_of_Adun, u);
						}
						else if (Broodwar->self()->allUnitCount(UnitTypes::Protoss_Robotics_Support_Bay) < supportBayDesired && Broodwar->self()->minerals() >= UnitTypes::Protoss_Robotics_Support_Bay.mineralPrice() && Broodwar->self()->gas() >= UnitTypes::Protoss_Robotics_Support_Bay.gasPrice())
						{
							buildingManager(UnitTypes::Protoss_Robotics_Support_Bay, u);
						}
						else if (Broodwar->self()->allUnitCount(UnitTypes::Protoss_Fleet_Beacon) < fleetBeaconDesired && Broodwar->self()->minerals() >= UnitTypes::Protoss_Fleet_Beacon.mineralPrice() && Broodwar->self()->gas() >= UnitTypes::Protoss_Fleet_Beacon.gasPrice())
						{
							buildingManager(UnitTypes::Protoss_Fleet_Beacon, u);
						}
						else if (Broodwar->self()->allUnitCount(UnitTypes::Protoss_Templar_Archives) < archivesDesired && Broodwar->self()->minerals() >= UnitTypes::Protoss_Templar_Archives.mineralPrice() && Broodwar->self()->gas() >= UnitTypes::Protoss_Templar_Archives.gasPrice())
						{
							buildingManager(UnitTypes::Protoss_Templar_Archives, u);
						}
						else if (Broodwar->self()->allUnitCount(UnitTypes::Protoss_Observatory) < observatoryDesired && Broodwar->self()->minerals() >= UnitTypes::Protoss_Observatory.mineralPrice() && Broodwar->self()->gas() >= UnitTypes::Protoss_Observatory.gasPrice())
						{
							buildingManager(UnitTypes::Protoss_Observatory, u);
						}
						else if (Broodwar->self()->allUnitCount(UnitTypes::Protoss_Arbiter_Tribunal) < tribunalDesired && Broodwar->self()->minerals() >= UnitTypes::Protoss_Arbiter_Tribunal.mineralPrice() && Broodwar->self()->gas() >= UnitTypes::Protoss_Arbiter_Tribunal.gasPrice())
						{
							buildingManager(UnitTypes::Protoss_Arbiter_Tribunal, u);
						}
						else if (Broodwar->self()->allUnitCount(UnitTypes::Protoss_Nexus) < nexusDesired + inactiveNexusCnt && Broodwar->self()->minerals() >= UnitTypes::Protoss_Nexus.mineralPrice())
						{
							for (int i = 0; i <= (int)nextExpansion.size() - 1; i++)
							{
								if (Broodwar->canBuildHere(nextExpansion.at(i), UnitTypes::Protoss_Nexus, u, false))
								{
									nexusManager(UnitTypes::Protoss_Nexus, u, nextExpansion.at(i));
									break;
								}
							}
						}
						else if (u->getID() == buildingWorkerID.front())
						{
							if (u->getLastCommandFrame() + 1000 < Broodwar->getFrameCount())
							{
								buildingWorkerID.pop_back();
							}
						}
					}
					// Scouter commands
					else if (u->getID() == scoutWorkerID.front() && BWTAhandling)
					{
						if (Broodwar->self()->supplyUsed() >= 18 && scouting)
						{
							if (enemyBasePositions.size() > 0 && u->getUnitsInRadius(256, Filter::IsEnemy && !Filter::IsWorker && Filter::CanAttack).size() < 1)
							{
								Position one = Position(enemyBasePositions.at(0).x - 192, enemyBasePositions.at(0).y - 192);
								Position two = Position(enemyBasePositions.at(0).x + 192, enemyBasePositions.at(0).y - 192);
								Position three = Position(enemyBasePositions.at(0).x + 192, enemyBasePositions.at(0).y + 192);
								Position four = Position(enemyBasePositions.at(0).x - 192, enemyBasePositions.at(0).y + 192);
								if (u->getDistance(one) < 64)
								{
									u->move(two);
								}
								else if (u->getDistance(two) < 64)
								{
									u->move(three);
								}
								else if (u->getDistance(three) < 64)
								{
									u->move(four);
								}
								else if (u->getDistance(four) < 64)
								{
									u->move(one);
								}
								else if (u->getLastCommandFrame() + 100 < Broodwar->getFrameCount())
								{
									u->move(one);
								}
							}
							else if (enemyBasePositions.size() < 1)
							{
								if (u->isCarryingMinerals())
								{
									if (u->getLastCommand().getType() != UnitCommandTypes::Return_Cargo)
									{
										u->returnCargo();
									}
									break;
								}
								for (auto start : getStartLocations())
								{
									if (Broodwar->isExplored(start->getTilePosition()) == false)
									{
										u->move(start->getPosition());
										break;
									}
								}
							}
							else
							{
								scouting = false;
							}
						}
						if (!scouting && !clearCut)
						{
							for (auto position : defendHere)
							{
								if (Broodwar->getUnitsInRadius(position, 128, Filter::IsMineralField).size() > 0 && !u->isGatheringMinerals())
								{
									clearCut = true;
									u->gather(Broodwar->getClosestUnit(position, Filter::IsMineralField));
									return;
								}
								else
								{
									clearCut = false;
								}
							}
						}
					}
					// Assign the probe a task (mineral, gas)
					assignProbe(u);
					continue;
				}				
			}
#pragma endregion
#pragma region Building Manager
			{

				// If it's a Nexus and we need probes, train the probes
				if (u->getType().isResourceDepot())
				{
					if (u->isIdle() && Broodwar->self()->allUnitCount(UnitTypes::Protoss_Probe) < 60 && (Broodwar->self()->minerals() >= UnitTypes::Protoss_Probe.mineralPrice() + queuedMineral) && Broodwar->self()->allUnitCount(UnitTypes::Protoss_Probe) < (int)((mineralID.size() * 2.5 + assimilatorID.size() * 3)))
					{
						u->train(UnitTypes::Protoss_Probe);
						continue;
					}
					// If there are no pylons around it, build one so we can make cannons
					if (u->getUnitsInRadius(128, Filter::GetType == UnitTypes::Enum::Protoss_Pylon).size() == 0 && Broodwar->self()->allUnitCount(UnitTypes::Protoss_Nexus) > 1)
					{
						TilePosition pylonNeeded = getBuildLocationNear(UnitTypes::Protoss_Pylon, Broodwar->getUnit(buildingWorkerID.front()), u->getTilePosition());
						Broodwar->getUnit(buildingWorkerID.front())->build(UnitTypes::Protoss_Pylon, pylonNeeded);
					}
					// If not at least two cannons, build two cannons, good for anti harass and detection
					if (u->getUnitsInRadius(640, Filter::GetType == UnitTypes::Enum::Protoss_Photon_Cannon).size() < 2 && Broodwar->self()->allUnitCount(UnitTypes::Protoss_Nexus) > 3)
					{
						TilePosition cannonNeeded = getBuildLocationNear(UnitTypes::Protoss_Photon_Cannon, Broodwar->getUnit(buildingWorkerID.front()), u->getTilePosition());
						Broodwar->getUnit(buildingWorkerID.front())->build(UnitTypes::Protoss_Photon_Cannon, cannonNeeded);
					}
					// If there are enemies nearby, request help
					if (u->getUnitsInRadius(320, Filter::IsEnemy).size() > 0)
					{
						enemyCountNearby = u->getUnitsInRadius(320, Filter::IsEnemy && !Filter::IsFlyer).size();
					}
					else
					{
						enemyCountNearby = 0;
					}
				}
				// If it's a building capable of production, send to production manager
				else if (u->getType().isBuilding() && u->getType() != UnitTypes::Protoss_Pylon && u->getType() != UnitTypes::Protoss_Nexus)
				{
					productionManager(u);
					continue;
				}				
			}
#pragma endregion

#pragma region Army Unit Manager	
			{
				if (u->getType() == UnitTypes::Protoss_Dragoon || u->getType() == UnitTypes::Protoss_Zealot || u->getType() == UnitTypes::Protoss_Dark_Templar)
				{
					unitGetCommand(u);
				}
				if (u->getType() == UnitTypes::Protoss_Shuttle)
				{
					shuttleManager(u);
					/*if (harassShuttleID.size() < 1 || find(harassShuttleID.begin(), harassShuttleID.end(), u->getID()) != harassShuttleID.end())
					{
					shuttleHarass(u);
					harassShuttleID.push_back(u->getID());
					}
					else
					{
					shuttleManager(u);
					}*/
				}
				if (u->getType() == UnitTypes::Protoss_Observer)
				{
					observerManager(u);
				}
				if (u->getType() == UnitTypes::Protoss_Reaver)
				{
					reaverManager(u);
					/*if (harassReaverID.size() < 1 || find(harassReaverID.begin(), harassReaverID.end(), u->getID()) != harassReaverID.end())
					{
					harassReaverID.push_back(u->getID());
					}*/
				}
				if (u->getType() == UnitTypes::Protoss_High_Templar)
				{
					templarManager(u);
				}
				if (u->getType() == UnitTypes::Protoss_Carrier)
				{
					carrierManager(u);
				}
				if (u->getType() == UnitTypes::Protoss_Arbiter)
				{
					arbiterManager(u);
				}
			}
#pragma endregion
		}
	}
#pragma endregion
}

void McRave::onSendText(std::string text)
{
	// Send the text to the game if it is not being processed.
	Broodwar->sendText("%s", text.c_str());
}

void McRave::onReceiveText(BWAPI::Player player, std::string text)
{
	// Parse the received text
	Broodwar << player->getName() << " said \"" << text << "\"" << std::endl;
}

void McRave::onPlayerLeft(BWAPI::Player player)
{
	// Interact verbally with the other players in the game by
	// announcing that the other player has left.
	Broodwar->sendText("GG %s!", player->getName().c_str());
}

void McRave::onNukeDetect(BWAPI::Position target)
{

	// Check if the target is a valid position
	if (target)
	{
		// if so, print the location of the nuclear strike target
		Broodwar << "Nuclear Launch Detected at " << target << std::endl;
	}
	else
	{
		// Otherwise, ask other players where the nuke is!
		Broodwar->sendText("Where's the nuke?");
	}

	// You can also retrieve all the nuclear missile targets using Broodwar->getNukeDots()!
}

void McRave::onUnitDiscover(BWAPI::Unit unit)
{
	if (unit->getPlayer() == Broodwar->enemy())
	{
		if (storeEnemyUnit(unit, enemyUnits) == 1)
		{
			if (!unit->getType().isWorker())
			{
				enemySupply += unit->getType().supplyRequired();
			}
			enemyStrength += unitGetStrength(unit);
			int enemyGateCnt = 0, enemyPoolCnt = 0, enemyRaxCnt = 0;
			if ((!twoGate || !fourPool || !twoRax) && scouting)
			{
				for (auto building : enemyUnits)
				{
					if (building.second.getUnitType() == UnitTypes::Protoss_Gateway)
					{
						enemyGateCnt++;
						// If two gateways and we haven't reported the pressure
						if (enemyGateCnt >= 2 && !twoGate)
						{
							twoGate = true;
							Broodwar << "Possible Early Gate Pressure" << endl;
						}
					}
					else if (building.second.getUnitType() == UnitTypes::Zerg_Spawning_Pool)
					{
						enemyPoolCnt++;
						// If early pool and we haven't reported the pressure
						if (enemyPoolCnt >= 1 && !fourPool)
						{
							fourPool = true;
							Broodwar << "Possible Early Pool Pressure" << endl;
						}
					}
					else if (building.second.getUnitType() == UnitTypes::Terran_Barracks)
					{
						enemyRaxCnt++;
						// If two barracks and we haven't reported the pressure
						if (enemyRaxCnt >= 2 && !twoRax)
						{
							twoRax = true;
							Broodwar << "Possible Early Barracks Pressure" << endl;
						}
						if (enemyRaxCnt == 0 && !twoRax)
						{
							twoRax = true;
							Broodwar << "Possible Proxy Barracks" << endl;
						}
					}
				}
			}
		}
	}
}

void McRave::onUnitEvade(BWAPI::Unit unit)
{
}

void McRave::onUnitShow(BWAPI::Unit unit)
{
}

void McRave::onUnitHide(BWAPI::Unit unit)
{
}

void McRave::onUnitCreate(BWAPI::Unit unit)
{
	if (unit->getPlayer() == Broodwar->self() && Broodwar->getFrameCount() >= 100)
	{
		if (unit->getType() == UnitTypes::Enum::Protoss_Nexus)
		{
			allyTerritory.push_back(BWTA::getRegion(unit->getPosition()));
			Position nexusPosition = unit->getPosition();
			// When Nexus is placed, get IDs of gas and minerals around Nexus so we can train probes ahead of time
			for (auto u : Broodwar->neutral()->getUnits())
			{
				if (u->getType() == UnitTypes::Resource_Vespene_Geyser && u->getDistance(nexusPosition) <= 400)
				{
					gasTilePosition.push_back(u->getTilePosition());
				}
				if (u->getType().isMineralField() == true && u->getDistance(nexusPosition) <= 400)
				{
					mineralID.push_back(u->getID());
				}
			}
		}
	}
}

void McRave::onUnitDestroy(BWAPI::Unit unit)
{
	if (unit->getPlayer() == Broodwar->self())
	{
		// Allied ground units
		if (unit->getType() == UnitTypes::Enum::Protoss_Probe)
		{
			if (unit->getID() == scoutWorkerID.front()){
				scoutWorkerID.pop_back();
				scouting = false;
			}
			if (unit->getID() == buildingWorkerID.front()){
				buildingWorkerID.pop_back();
			}
			if (find(mineralWorkerID.begin(), mineralWorkerID.end(), unit->getID()) != mineralWorkerID.end())
			{
				mineralWorkerID.erase(find(mineralWorkerID.begin(), mineralWorkerID.end(), unit->getID()));
			}
			if (find(gasWorkerID.begin(), gasWorkerID.end(), unit->getID()) != gasWorkerID.end())
			{
				gasWorkerID.erase(find(gasWorkerID.begin(), gasWorkerID.end(), unit->getID()));
			}
			if (find(combatWorkerID.begin(), combatWorkerID.end(), unit->getID()) != combatWorkerID.end())
			{
				combatWorkerID.erase(find(combatWorkerID.begin(), combatWorkerID.end(), unit->getID()));
			}
		}
		else if (unit->getType() == UnitTypes::Enum::Protoss_Reaver && find(reaverID.begin(), reaverID.end(), unit->getID()) != reaverID.end())
		{
			reaverID.erase(find(reaverID.begin(), reaverID.end(), unit->getID()));
		}
		else if (unit->getType() == UnitTypes::Enum::Protoss_Shuttle && find(shuttleID.begin(), shuttleID.end(), unit->getID()) != shuttleID.end())
		{
			shuttleID.erase(find(shuttleID.begin(), shuttleID.end(), unit->getID()));
		}
		if (!unit->getType().isWorker())
		{
			allySupply = allySupply - unit->getType().supplyRequired();
		}
	}

	else if (unit->getPlayer() == Broodwar->enemy())
	{
		// Strength and Supply
		if (!unit->getType().isWorker())
		{
			enemySupply -= unit->getType().supplyRequired();
		}
		enemyStrength -= unitGetStrength(unit);
	}
	// If a mineral field died that we are keeping track of, remove the mineral from the vector and the corresponding worker(s) that were tasked on it
	// Removing the workers means we can re-assign them to new tasks immediately where necessary
	if (unit->getType().isMineralField() && find(mineralID.begin(), mineralID.end(), unit->getID()) != mineralID.end())
	{
		int pointer = find(mineralID.begin(), mineralID.end(), unit->getID()) - mineralID.begin();
		if ((find(probeID.begin(), probeID.end(), mineralWorkerID.at(pointer * 2)) != probeID.end()))
		{
			probeID.erase(find(probeID.begin(), probeID.end(), mineralWorkerID.at(pointer * 2)), find(probeID.begin(), probeID.end(), mineralWorkerID.at((pointer * 2) + 1)));
			mineralWorkerID.erase(find(mineralWorkerID.begin(), mineralWorkerID.end(), mineralWorkerID.at(pointer * 2)), find(mineralWorkerID.begin(), mineralWorkerID.end(), mineralWorkerID.at((pointer * 2) + 1)));
			mineralID.erase(find(mineralID.begin(), mineralID.end(), unit->getID()));
		}
		// Remove this nexus as an active expansion, so we can expand to a new base to keep our income up
		TilePosition closestNexus = Broodwar->getClosestUnit(unit->getPosition(), Filter::IsResourceDepot)->getTilePosition();
		if (find(activeExpansion.begin(), activeExpansion.end(), closestNexus) != activeExpansion.end())
		{
			activeExpansion.erase(find(activeExpansion.begin(), activeExpansion.end(), closestNexus));
			inactiveNexusCnt++;
		}
	}
	if (unit->getPlayer() == Broodwar->enemy() && unit->getType().isResourceDepot() && find(enemyBasePositions.begin(), enemyBasePositions.end(), unit->getPosition()) != enemyBasePositions.end())
	{
		enemyBasePositions.erase(find(enemyBasePositions.begin(), enemyBasePositions.end(), unit->getPosition()));
	}
}

void McRave::onUnitMorph(BWAPI::Unit unit)
{
}

void McRave::onUnitRenegade(BWAPI::Unit unit)
{
}

void McRave::onSaveGame(std::string gameName)
{
	Broodwar << "The game was saved to \"" << gameName << "\"" << std::endl;
}

void McRave::onUnitComplete(BWAPI::Unit unit)
{
	// If player owned unit is completed, get counter
	if (unit->getPlayer() == Broodwar->self())
	{
		// If it's the start of the game, gather the IDs of the gas and minerals around the Nexus, otherwise this is done during the Unit Create function
		if (unit->getType() == UnitTypes::Enum::Protoss_Nexus && Broodwar->getFrameCount() < 100)
		{
			Position nexusPosition = unit->getPosition();
			for (auto u : Broodwar->neutral()->getUnits())
			{
				if (u->getType() == UnitTypes::Resource_Vespene_Geyser && u->getDistance(nexusPosition) <= 400)
				{
					gasTilePosition.push_back(u->getTilePosition());
				}
				if (u->getType().isMineralField() == true && u->getDistance(nexusPosition) <= 400)
				{
					mineralID.push_back(u->getID());
				}
			}
		}
		else if (unit->getType() == UnitTypes::Enum::Protoss_Assimilator)
		{
			assimilatorID.push_back(unit->getID());
		}
		if (!unit->getType().isWorker())
		{
			allySupply = allySupply + unit->getType().supplyRequired();
		}
	}

	// If unit not owned by player
	if (unit->getPlayer()->getID() == Broodwar->enemy()->getID())
	{
		if (unit->getType().isBuilding())
		{
			if (enemyBasePositions.size() == 0)
			{
				enemyStartingTilePosition = getNearestBaseLocation(unit->getPosition())->getTilePosition();
				enemyStartingPosition = Position(enemyStartingTilePosition.x * 32, enemyStartingTilePosition.y * 32);
				path = theMap.GetPath(playerStartingPosition, enemyStartingPosition);

				// For each chokepoint, set a 10 tile radius of "no fly zone"
				for (auto position : path)
				{
					// Convert walk position (8x8) to tile position (32x32) = divide by 4
					int x = (position->Center().x / 4);
					int y = (position->Center().y / 4);

					int x1 = (position->Center().x / 4) - 15;
					int y1 = (position->Center().y / 4) - 15;

					for (int i = x1; i <= x1 + 30; i++)
					{
						for (int j = y1; j <= y1 + 30; j++)
						{
							if ((x >= 0 && x <= Broodwar->mapWidth() && y >= 0 && y <= Broodwar->mapHeight()) && TilePosition(i, j).getDistance(TilePosition(x, y)) < 320)
							{
								shuttleHeatmap[i][j] = 256;
							}
						}
					}
				}
			}
		}
		if (unit->getType().isResourceDepot() && find(enemyBasePositions.begin(), enemyBasePositions.end(), unit->getPosition()) == enemyBasePositions.end())
		{
			enemyBasePositions.push_back(unit->getPosition());
		}
	}
}

DWORD WINAPI AnalyzeThread()
{
	BWTA::analyze();

	analyzed = true;
	analysis_just_finished = true;
	return 0;
}

void McRave::drawTerrainData()
{
	//we will iterate through all the base locations, and draw their outlines.
	for (const auto& baseLocation : BWTA::getBaseLocations()) {
		TilePosition p = baseLocation->getTilePosition();

		//draw outline of center location
		Position leftTop(p.x * TILE_SIZE, p.y * TILE_SIZE);
		Position rightBottom(leftTop.x + 4 * TILE_SIZE, leftTop.y + 3 * TILE_SIZE);
		Broodwar->drawBoxMap(leftTop, rightBottom, Colors::Blue);

		//draw a circle at each mineral patch
		for (const auto& mineral : baseLocation->getStaticMinerals()) {
			Broodwar->drawCircleMap(mineral->getInitialPosition(), 30, Colors::Cyan);
		}

		//draw the outlines of Vespene geysers
		for (const auto& geyser : baseLocation->getGeysers()) {
			TilePosition p1 = geyser->getInitialTilePosition();
			Position leftTop1(p1.x * TILE_SIZE, p1.y * TILE_SIZE);
			Position rightBottom1(leftTop1.x + 4 * TILE_SIZE, leftTop1.y + 2 * TILE_SIZE);
			Broodwar->drawBoxMap(leftTop1, rightBottom1, Colors::Orange);
		}

		//if this is an island expansion, draw a yellow circle around the base location
		if (baseLocation->isIsland()) {
			Broodwar->drawCircleMap(baseLocation->getPosition(), 80, Colors::Yellow);
		}
	}

	//we will iterate through all the regions and ...
	for (const auto& region : BWTA::getRegions()) {
		// draw the polygon outline of it in green
		BWTA::Polygon p = region->getPolygon();
		for (size_t j = 0; j < p.size(); ++j) {
			Position point1 = p[j];
			Position point2 = p[(j + 1) % p.size()];
			Broodwar->drawLineMap(point1, point2, Colors::Green);
		}
		// visualize the chokepoints with red lines
		for (auto const& chokepoint : region->getChokepoints()) {
			Position point1 = chokepoint->getSides().first;
			Position point2 = chokepoint->getSides().second;
			Broodwar->drawLineMap(point1, point2, Colors::Red);
		}
	}
}