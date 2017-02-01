// CMProtoBot is made by Christian McCrave
// For any questions, email christianmccrave@gmail.com
// Bot started 01/03/2017 - currently in Alpha

// Include header with all includes in them
#include "Header.h"
#include "Global.h"

// Namespaces
using namespace std;
using namespace BWAPI;
using namespace BWTA;
using namespace Filter;

bool analyzed;
bool analysis_just_finished;

void CMProtoBot::onStart()
{
	// Enable the UserInput flag, which allows us to control the bot and type messages.
	Broodwar->enableFlag(Flag::UserInput);

	// Set the command optimization level so that common commands can be grouped and reduce the bot's APM (Actions Per Minute).
	Broodwar->setCommandOptimizationLevel(2);

	if (Broodwar->enemy())
	{
		Broodwar << "The matchup is " << Broodwar->self()->getRace() << " vs " << Broodwar->enemy()->getRace() << endl;
	}
	if (analyzed == false) {
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AnalyzeThread, NULL, 0, NULL);
	}

	readMap();
	analyzed = false;
	analysis_just_finished = false;
}

void CMProtoBot::onEnd(bool isWinner)
{
	// Called when the game ends
	if (isWinner)
	{
		// Log your win here!
	}
}

void CMProtoBot::onFrame()
{
	// Threat level manager (TESTING)
	for (int x = 0; x <= Broodwar->mapWidth(); x++)
	{
		for (int y = 0; y <= Broodwar->mapHeight(); y++)
		{
			if (threatArray[x][y] > 0)
			{
				Broodwar->drawTextMap(x * 32, y * 32, "%d", threatArray[x][y]);
				threatArray[x][y] = 0;
			}
		}
	}
	for (auto &u : Broodwar->enemy()->getUnits())
	{
		if (!u->getType().isBuilding() && u->getType().groundWeapon().damageAmount() > 0)
		{			
			int range = u->getType().groundWeapon().maxRange()/32;
			// The + 1 is because we need to still check an additional tile
			for (int x = u->getTilePosition().x - range; x <= u->getTilePosition().x + range + 1; x++)
			{
				for (int y = u->getTilePosition().y - range; y <= u->getTilePosition().y + range + 1; y++)
				{
					if ((u->getDistance(Position((x * 32), (y * 32))) <= (range*32)) && (x > 0 || x <= Broodwar->mapWidth() || y > 0 || y <= Broodwar->mapHeight()))
					{
						threatArray[x][y] += u->getType().groundWeapon().damageAmount();
					}				
				}
			}
		}
	}

	// BWTA draw
	if (analyzed)
		drawTerrainData();

	// Only do this loop once if map analysis done
	if (analysis_just_finished)
	{
		//Broodwar << "Finished analyzing map." << std::endl;
		analysis_just_finished = false;

		// --------------------------------------------------------------------------------------------------------------------------------------------
		// Base Locations
		// --------------------------------------------------------------------------------------------------------------------------------------------

		// Find player starting position and tile position		
		BaseLocation* playerStartingLocation = getStartLocation(Broodwar->self());
		playerStartingPosition = playerStartingLocation->getPosition();
		playerStartingTilePosition = playerStartingLocation->getTilePosition();

		// Find all start locations
		set<BWTA::BaseLocation*> startLocations = BWTA::getStartLocations();
		for (set<BWTA::BaseLocation*>::iterator itr = startLocations.begin(); itr != startLocations.end(); itr++)
		{
			startingLocationPositions.push_back((*itr)->getPosition());
			startingLocationTilePositions.push_back((*itr)->getTilePosition());
		}

		// Find base locations, find positions and tilepositions of each base location, put those in a vector for easier use
		std::set<BWTA::BaseLocation*> bases = BWTA::getBaseLocations();
		for (std::set<BWTA::BaseLocation*>::iterator itr = bases.begin(); itr != bases.end(); itr++)
		{
			if (!(*itr)->isIsland())
			{
				// Get base positions and tile positions, store in vectors
				basePositions.push_back((*itr)->getPosition());
				baseTilePositions.push_back((*itr)->getTilePosition());

				// Get closest locations to player starting tile position
				baseDistances.push_back(BWTA::getGroundDistance(playerStartingTilePosition, (*itr)->getTilePosition()));
				baseDistancesBuffer.push_back(BWTA::getGroundDistance(playerStartingTilePosition, (*itr)->getTilePosition()));
			}
		}

		// Find nearest bases, move to vector for easier use (TESTING ONLY HALF THE BASES, WANT ENEMY EXPANSIONS SEPARATE)
		for (int i = 0; i < ((int)baseDistances.size() / (int)(Broodwar->getPlayers().size() - 1)); i++)
		{
			nearestBases.push_back(*min_element(baseDistancesBuffer.begin(), baseDistancesBuffer.end()));
			furthestBases.push_back(*max_element(baseDistancesBuffer.begin(), baseDistancesBuffer.end()));
			baseDistancesBuffer.erase(min_element(baseDistancesBuffer.begin(), baseDistancesBuffer.end()));
			baseDistancesBuffer.erase(max_element(baseDistancesBuffer.begin(), baseDistancesBuffer.end()));

			// Reorganize base positions and tile positions into ascending order of distance for expansions
			nearestBasePositions.push_back(basePositions.at((find(baseDistances.begin(), baseDistances.end(), nearestBases.at(i)) - baseDistances.begin())));
			nearestBaseTilePositions.push_back(baseTilePositions.at((find(baseDistances.begin(), baseDistances.end(), nearestBases.at(i))) - baseDistances.begin()));
			nearestBaseTilePositionsBuffer.push_back(baseTilePositions.at((find(baseDistances.begin(), baseDistances.end(), nearestBases.at(i))) - baseDistances.begin()));

			// Reorganize base positions and tile positions into descending order of distance for enemy expansions  (IMPLEMENTING)
			// furthestBasePositions.push_back()
		}

		// Check each base tile position, if they overlap, erase one
		for (int i = 0; i <= (int)nearestBaseTilePositions.size() - 2; i++)
		{
			if (abs(nearestBaseTilePositions.at(i).x - nearestBaseTilePositions.at(i + 1).x) <= 10 && abs(nearestBaseTilePositions.at(i).y - nearestBaseTilePositions.at(i + 1).y) <= 10)
			{
				nearestBaseTilePositions.erase(nearestBaseTilePositions.begin() + i);
				nearestBaseTilePositionsBuffer.erase(nearestBaseTilePositionsBuffer.begin() + i);
			}
		}

		// Erase starting locations from base positions for better scouting/expanding utility
		nextExpansion.push_back(nearestBaseTilePositions.at(0));
		activeExpansion.push_back(nearestBaseTilePositions.at(0));
		nearestBasePositions.erase(nearestBasePositions.begin());
		startingLocationPositions.erase(find(startingLocationPositions.begin(), startingLocationPositions.end(), playerStartingPosition));
		startingLocationTilePositions.erase(find(startingLocationTilePositions.begin(), startingLocationTilePositions.end(), playerStartingTilePosition));

		// For each expansion
		for (int i = 0; i <= (int)nearestBaseTilePositions.size() - 2; i++)
		{
			nearestBaseTilePositionsBuffer.erase(find(nearestBaseTilePositionsBuffer.begin(), nearestBaseTilePositionsBuffer.end(), nextExpansion.at(i)));
			//Look at remaining base locations, closest one is next expansion
			for (int j = 0; j <= (int)nearestBaseTilePositionsBuffer.size() - 2; j++)
			{
				double rawDistance = getGroundDistance(nextExpansion.at(i), nearestBaseTilePositionsBuffer.at(j));
				expansionRawDistance.push_back(rawDistance);
			}

			nextExpansion.push_back(nearestBaseTilePositionsBuffer.at(min_element(expansionRawDistance.begin(), expansionRawDistance.end()) - expansionRawDistance.begin()));
			activeExpansion.push_back(nearestBaseTilePositionsBuffer.at(min_element(expansionRawDistance.begin(), expansionRawDistance.end()) - expansionRawDistance.begin()));

			// Based on the next expansion, get the distance to our starting location so we can see if any other expansions are closer
			double expectedDistance = sqrt(pow((playerStartingTilePosition.x - nextExpansion.back().x), 2) + pow((playerStartingTilePosition.y - nextExpansion.back().y), 2));

			// Measure distance to every possible expansion		
			for (int j = 0; j <= (int)nearestBaseTilePositionsBuffer.size() - 2; j++)
			{
				double startDistance = pow(playerStartingTilePosition.x - (nearestBaseTilePositionsBuffer.at(j)).x, 2)
					+ pow(playerStartingTilePosition.y - (nearestBaseTilePositionsBuffer.at(j)).y, 2);
				expansionStartDistance.push_back(sqrt(startDistance));
			}

			// Check to see if it's closer, if so, add that as next expansion
			if ((*min_element(expansionStartDistance.begin(), expansionStartDistance.end()) * 2) <= expectedDistance)
			{
				nextExpansion.pop_back();
				nextExpansion.push_back(nearestBaseTilePositionsBuffer.at(min_element(expansionStartDistance.begin(), expansionStartDistance.end()) - expansionStartDistance.begin()));
				activeExpansion.pop_back();
				activeExpansion.push_back(nearestBaseTilePositionsBuffer.at(min_element(expansionStartDistance.begin(), expansionStartDistance.end()) - expansionStartDistance.begin()));
			}
			expansionRawDistance.erase(expansionRawDistance.begin(), expansionRawDistance.end());
			expansionStartDistance.erase(expansionStartDistance.begin(), expansionStartDistance.end());
		}

		// --------------------------------------------------------------------------------------------------------------------------------------------
		// Chokepoint Locations
		// --------------------------------------------------------------------------------------------------------------------------------------------

		// Find chokepoints, move to vector for easier use 
		set<BWTA::Chokepoint*> myChokes = BWTA::getChokepoints();
		for (std::set<BWTA::Chokepoint*>::iterator itr = myChokes.begin(); itr != myChokes.end(); itr++)
		{			
			chokepointPositions.push_back((*itr)->getCenter());
			chokepointDistances.push_back(BWTA::getGroundDistance(playerStartingTilePosition, TilePosition(chokepointPositions.back())));
			chokepointDistancesBuffer.push_back(BWTA::getGroundDistance(playerStartingTilePosition, TilePosition(chokepointPositions.back())));			
		}
		// Find nearest chokepoints, move to vector for easier use
		for (int i = 0; i <= (int)chokepointDistances.size() - 1; i++)
		{
			lowestChokepointDistance.push_back(*std::min_element(chokepointDistancesBuffer.begin(), chokepointDistancesBuffer.end()));
			chokepointDistancesBuffer.erase(std::min_element(chokepointDistancesBuffer.begin(), chokepointDistancesBuffer.end()));
			// Reorganize chokepoint positions and tile positions into ascending order of distance
			nearestChokepointPosition.push_back(chokepointPositions.at((find(chokepointDistances.begin(), chokepointDistances.end(), lowestChokepointDistance.at(i)) - chokepointDistances.begin())));
		}
		BWTAhandling = true;
	}
	if (BWTAhandling)
	{
		for (int i = 0; i <= (int)nearestChokepointPosition.size() - 1; i++)
		{
			Broodwar->drawCircleMap(nearestChokepointPosition.at(i), 100, Colors::White);
		}
		for (int i = 0; i <= (int)activeExpansion.size() - 1; i++)
		{
			Broodwar->drawTextMap(activeExpansion.at(i).x * 32, activeExpansion.at(i).y * 32, "Base %d", i, Colors::White);
		}

		Broodwar->drawTextMap(enemyStartingPosition, "Starting Position");
		// Holding position, average between all chokepoints
		/*int xC = 0;
		int yC = 0;
		set <Chokepoint*> getRegionChokes = getRegion(furthestNexus)->getChokepoints();
		for (set <Chokepoint*>::iterator itr = getRegionChokes.begin(); itr != getRegionChokes.end(); itr++)
		{
		xC = xC + (*itr)->getCenter().x;
		yC = yC + (*itr)->getCenter().y;
		}
		holdingPosition.x = xC / getRegionChokes.size();
		holdingPosition.y = yC / getRegionChokes.size();
		Broodwar->drawTextMap(furthestNexus.x *32, furthestNexus.y * 32 + 10, "Defend Point");*/
		//holdingPosition = Broodwar->getRegionAt(Position(32 * nextExpansion.at(1).x, 32 * nextExpansion.at(1).y))->getCenter();
		if (Broodwar->self()->visibleUnitCount() > 0)
		{
			holdingPosition = Position(furthestNexus.x * 32, furthestNexus.y * 32);
		}
		Broodwar->drawCircleMap(holdingPosition, 200, Colors::Blue, false);
		// Get build order based on enemy race(s)
		getBuildOrder();
	}
	// --------------------------------------------------------------------------------------------------------------------------------------------
	// On-Screen Information
	// --------------------------------------------------------------------------------------------------------------------------------------------

	//// Display some information about our buildings
	//Broodwar->drawTextScreen(0, 0, "Building Count/Desired");
	//Broodwar->drawTextScreen(0, 10, "Nexus:");
	//Broodwar->drawTextScreen(0, 20, "Pylon:");
	//Broodwar->drawTextScreen(0, 30, "Gas:");
	//Broodwar->drawTextScreen(0, 40, "Gate:");
	//Broodwar->drawTextScreen(0, 50, "Forge:");
	//Broodwar->drawTextScreen(0, 60, "Core:");
	//Broodwar->drawTextScreen(0, 70, "RoboF:");
	//Broodwar->drawTextScreen(0, 80, "Stargate:");
	//Broodwar->drawTextScreen(0, 90, "Citadel:");
	//Broodwar->drawTextScreen(0, 100, "Support:");
	//Broodwar->drawTextScreen(0, 110, "Fleet:");
	//Broodwar->drawTextScreen(0, 120, "Archives:");

	//// Counters
	//Broodwar->drawTextScreen(0, 0, "Building Count/Desired");
	//Broodwar->drawTextScreen(50, 10, "%d  %d Inactive: %d", nexusCnt, nexusDesired, inactiveNexusCnt);
	//Broodwar->drawTextScreen(50, 20, "%d  %d", pylonCnt, pylonDesired);
	//Broodwar->drawTextScreen(50, 30, "%d  %d", gasCnt, gasDesired);
	//Broodwar->drawTextScreen(50, 40, "%d  %d", gateCnt, gateDesired);
	//Broodwar->drawTextScreen(50, 50, "%d  %d", forgeCnt, forgeDesired);
	//Broodwar->drawTextScreen(50, 60, "%d  %d", coreCnt, coreDesired);
	//Broodwar->drawTextScreen(50, 70, "%d  %d", roboCnt, roboDesired);
	//Broodwar->drawTextScreen(50, 80, "%d  %d", stargateCnt, stargateDesired);
	//Broodwar->drawTextScreen(50, 90, "%d  %d", citadelCnt, citadelDesired);
	//Broodwar->drawTextScreen(50, 100, "%d  %d", supportBayCnt, supportBayDesired);
	//Broodwar->drawTextScreen(50, 110, "%d  %d", fleetBeaconCnt, fleetBeaconDesired);
	//Broodwar->drawTextScreen(50, 120, "%d  %d", archivesCnt, archivesDesired);

	// Display some information about our queued resources required for structure building
	Broodwar->drawTextScreen(200, 0, "Queued Minerals: %d", queuedMineral);
	Broodwar->drawTextScreen(200, 10, "Queued Gas: %d", queuedGas);

	//// Display some information about our units
	//Broodwar->drawTextScreen(500, 20, "Unit Count");
	//Broodwar->drawTextScreen(500, 30, "Probe Count: %d", Broodwar->self()->allUnitCount(UnitTypes::Protoss_Probe));
	//Broodwar->drawTextScreen(500, 40, "Zealot Count: %d", zealotCnt);
	//Broodwar->drawTextScreen(500, 50, "Dragoon Count: %d", dragoonCnt);
	//Broodwar->drawTextScreen(500, 60, "Carrier Count: %d", carrierCnt);

	// Display some information for debuffing
	Broodwar->drawTextScreen(500, 200, "Mineral ID Size: %d", mineralID.size());
	Broodwar->drawTextScreen(500, 210, "Mineral Worker ID Size: %d", mineralWorkerID.size());
	Broodwar->drawTextScreen(500, 220, "Gas ID Size: %d", assimilatorID.size());
	Broodwar->drawTextScreen(500, 230, "Gas Worker ID Size: %d", gasWorkerID.size());
	Broodwar->drawTextScreen(500, 240, "Combat Worker ID Size: %d", combatWorkerID.size());

	//
	Broodwar->drawTextScreen(500, 250, "Our Army: %d", allySupply);
	Broodwar->drawTextScreen(500, 260, "Their Army: %d", enemySupply);	
	Broodwar->drawTextScreen(500, 280, "Cnt: %d", enemyCountNearby);


	// Display which worker is builder and scouter (slightly delayed)
	if (Broodwar->getFrameCount() > 100)
	{
		Broodwar->drawTextMap(Broodwar->getUnit(buildingWorkerID.front())->getPosition(), "Builder", Colors::Yellow);
		Broodwar->drawTextMap(Broodwar->getUnit(scoutWorkerID.front())->getPosition(), "Scouter", Colors::Yellow);
	}

	TilePosition expectedRightCorner;
	expectedRightCorner.x = buildTilePosition.x + currentBuilding.tileWidth();
	expectedRightCorner.y = buildTilePosition.y + currentBuilding.tileHeight();
	Broodwar->drawBoxMap(buildTilePosition.x * 32, buildTilePosition.y * 32, expectedRightCorner.x * 32, expectedRightCorner.y * 32, Colors::Black, false);


	// --------------------------------------------------------------------------------------------------------------------------------------------
	// Structure Information
	// --------------------------------------------------------------------------------------------------------------------------------------------


	// If a structure is required, make sure we notify production that we have an additional cost to not eat into
	queuedMineral = std::max(0, (nexusDesired - Broodwar->self()->allUnitCount(UnitTypes::Protoss_Nexus))*(UnitTypes::Protoss_Nexus.mineralPrice()))
		+ std::max(0, (pylonDesired - Broodwar->self()->allUnitCount(UnitTypes::Protoss_Pylon))*(UnitTypes::Protoss_Pylon.mineralPrice()))
		+ std::max(0, (gasDesired - Broodwar->self()->allUnitCount(UnitTypes::Protoss_Assimilator))*(UnitTypes::Protoss_Assimilator.mineralPrice()))
		+ std::max(0, (gateDesired - Broodwar->self()->allUnitCount(UnitTypes::Protoss_Gateway))*(UnitTypes::Protoss_Gateway.mineralPrice()))
		+ std::max(0, (forgeDesired - Broodwar->self()->allUnitCount(UnitTypes::Protoss_Forge))*(UnitTypes::Protoss_Forge.mineralPrice()))
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

	// --------------------------------------------------------------------------------------------------------------------------------------------
	// Probe Manager
	// --------------------------------------------------------------------------------------------------------------------------------------------

	// Prevent spamming by only running our onFrame once every number of latency frames.
	// Latency frames are the number of frames before commands are processed.
	if (Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0)
		return;

	// Iterate through all the units that we own
	for (auto &u : Broodwar->self()->getUnits())
	{
		// Ignore the unit if it no longer exists, is locked down, maelstrommed, stassised, loaded, not powered, stuck, not completed
		if (!u->exists() || u->isLockedDown() || u->isMaelstrommed() || u->isStasised()
			|| u->isLoaded() || !u->isPowered() || u->isStuck() || !u->isCompleted())
			continue;

		// Probe commands
		if (u->getType() == UnitTypes::Protoss_Probe)
		{			
			// Assign the probe a task (mineral, gas)
			assignProbe(u);

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
				builderPosition = u->getPosition();
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
						if (Broodwar->isBuildable(nextExpansion.at(i), true))
						{
							nexusManager(UnitTypes::Protoss_Nexus, u, nextExpansion.at(i));
							break;
						}
					}
				}
			}
			else if (u->getID() == buildingWorkerID.front() && u->isStuck())
			{
				if (u->getLastCommandFrame() > Broodwar->getFrameCount() + 1000)
				{
					buildingWorkerID.pop_back();
				}
			}
			// Scouter commands
			else if (u->getID() == scoutWorkerID.front() && BWTAhandling)
			{
				if (Broodwar->self()->supplyUsed() >= 18)
				{
					for (int i = 0; i <= (int)startingLocationPositions.size() - 1; i++)
					{
						if (Broodwar->isExplored(startingLocationTilePositions.at(i)) == false)
						{
							u->move(startingLocationPositions.at(i));
						}
					}
					/*for (int i = 1; i <= (int)nextExpansion.size() - 1; i++)
					{
						if (Broodwar->isExplored(nextExpansion.at(i).x + 2, nextExpansion.at(i).y + 2) == false)
						{
							u->move(Position(32 * (nextExpansion.at(i).x), 32 * (nextExpansion.at(i).y + 4)), true);
							u->move(Position(32 * (nextExpansion.at(i).x + 4), 32 * (nextExpansion.at(i).y + 4)), true);
							break;
						}
					}*/
				}
			}
			if (enemyBasePositions.size() < 1 && Broodwar->getFrameCount() > 15000)
			{
				for (int i = 0; i <= basePositions.size() - 1; i++)
				{
					if (!Broodwar->isVisible(TilePosition(basePositions.at(i))))
					{
						Broodwar->getClosestUnit(basePositions.at(i), Filter::IsAlly && !Filter::IsBuilding)->attack(basePositions.at(i));
					}
				}
			}
		}

		// --------------------------------------------------------------------------------------------------------------------------------------------
		// Building Manager
		// --------------------------------------------------------------------------------------------------------------------------------------------

		// If it's a Nexus and we need probes, train the probes
		else if (u->getType().isResourceDepot())
		{
			if (u->isIdle() && Broodwar->self()->allUnitCount(UnitTypes::Protoss_Probe) < 60 && (Broodwar->self()->minerals() >= UnitTypes::Protoss_Probe.mineralPrice() + queuedMineral) && Broodwar->self()->allUnitCount(UnitTypes::Protoss_Probe) < (int)(mineralID.size() * 2 + assimilatorID.size() * 3))
			{
				u->train(UnitTypes::Protoss_Probe);
				continue;
			}
			// If there are no pylons around it, build one so we can make cannons
			if (u->getUnitsInRadius(100, Filter::GetType == UnitTypes::Enum::Protoss_Pylon).size() == 0 && Broodwar->self()->allUnitCount(UnitTypes::Protoss_Nexus) > 1)
			{
				pylonNeeded = getBuildLocationNear(UnitTypes::Protoss_Pylon, Broodwar->getUnit(buildingWorkerID.front()), u->getTilePosition());
				Broodwar->getUnit(buildingWorkerID.front())->build(UnitTypes::Protoss_Pylon, pylonNeeded);
			}
			// If not at least two cannons, build two cannons, good for anti harass and detection
			if (u->getUnitsInRadius(500, Filter::GetType == UnitTypes::Enum::Protoss_Photon_Cannon).size() < 2 && Broodwar->self()->allUnitCount(UnitTypes::Protoss_Nexus) > 1)
			{
				cannonNeeded = getBuildLocationNear(UnitTypes::Protoss_Photon_Cannon, Broodwar->getUnit(buildingWorkerID.front()), u->getTilePosition());
				Broodwar->getUnit(buildingWorkerID.front())->build(UnitTypes::Protoss_Photon_Cannon, cannonNeeded);
			}
			// If there are enemies nearby, request help
			if (u->getUnitsInRadius(320, Filter::IsEnemy).size() > 0)
			{
				enemyCountNearby = u->getUnitsInRadius(320, Filter::IsEnemy && !Filter::IsFlyer).size();
			}
		}
		// If it's a building capable of production, send to production manager
		else if (u->getType().isBuilding() && u->getType() != UnitTypes::Protoss_Pylon && u->getType() != UnitTypes::Protoss_Nexus)
		{
			productionManager(u);
		}

		// --------------------------------------------------------------------------------------------------------------------------------------------
		// Unit Manager
		// --------------------------------------------------------------------------------------------------------------------------------------------

		if (u->getType() == UnitTypes::Protoss_Dragoon || u->getType() == UnitTypes::Protoss_Zealot || u->getType() == UnitTypes::Protoss_Dark_Templar)
		{
			unitGetCommand(u);
		}
		if (u->getType() == UnitTypes::Protoss_Shuttle)
		{
			shuttleManager(u);
		}
		if (u->getType() == UnitTypes::Protoss_Observer)
		{
			observerManager(u);
		}
		if (u->getType() == UnitTypes::Protoss_Reaver)
		{
			reaverManager(u);
		}
		if (u->getType() == UnitTypes::Protoss_Carrier)
		{
			carrierManager(u);
		}

		// Calculating my current army supply
		allySupply = Broodwar->self()->allUnitCount() - Broodwar->self()->allUnitCount(UnitTypes::Buildings) - Broodwar->self()->allUnitCount(UnitTypes::Protoss_Probe);
	}	
}

void CMProtoBot::onSendText(std::string text)
{
	// Send the text to the game if it is not being processed.
	Broodwar->sendText("%s", text.c_str());
}

void CMProtoBot::onReceiveText(BWAPI::Player player, std::string text)
{
	// Parse the received text
	Broodwar << player->getName() << " said \"" << text << "\"" << std::endl;
}

void CMProtoBot::onPlayerLeft(BWAPI::Player player)
{
	// Interact verbally with the other players in the game by
	// announcing that the other player has left.
	Broodwar->sendText("GG %s!", player->getName().c_str());
}

void CMProtoBot::onNukeDetect(BWAPI::Position target)
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

void CMProtoBot::onUnitDiscover(BWAPI::Unit unit)
{
}

void CMProtoBot::onUnitEvade(BWAPI::Unit unit)
{
}

void CMProtoBot::onUnitShow(BWAPI::Unit unit)
{
}

void CMProtoBot::onUnitHide(BWAPI::Unit unit)
{
}

void CMProtoBot::onUnitCreate(BWAPI::Unit unit)
{
	if (unit->getPlayer() == Broodwar->self() && Broodwar->getFrameCount() >= 100)
	{
		if (unit->getType() == UnitTypes::Enum::Protoss_Nexus)
		{
			Position nexusPosition = unit->getPosition();
			if (getGroundDistance(unit->getTilePosition(), enemyStartingTilePosition) < getGroundDistance(furthestNexus, enemyStartingTilePosition))
			{
				furthestNexus = unit->getTilePosition();
			}
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

void CMProtoBot::onUnitDestroy(BWAPI::Unit unit)
{
	if (unit->getPlayer() == Broodwar->self() && !unit->isConstructing())
	{
		// Allied ground units
		if (unit->getType() = UnitTypes::Enum::Protoss_Probe)
		{
			if (unit->getID() == scoutWorkerID.front()){
				scoutWorkerID.pop_back();
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
	}
	if (unit->getPlayer() == Broodwar->enemy())
	{
		if (!unit->getType().isWorker())
		{
			enemySupply = enemySupply - unit->getType().supplyRequired();
		}
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

void CMProtoBot::onUnitMorph(BWAPI::Unit unit)
{
}

void CMProtoBot::onUnitRenegade(BWAPI::Unit unit)
{
}

void CMProtoBot::onSaveGame(std::string gameName)
{
	Broodwar << "The game was saved to \"" << gameName << "\"" << std::endl;
}

void CMProtoBot::onUnitComplete(BWAPI::Unit unit)
{
	// If player owned unit is completed, get counter
	if (unit->getPlayer() == Broodwar->self())
	{
		// If it's the start of the game, gather the IDs of the gas and minerals around the Nexus, otherwise this is done during the Unit Create function
		if (unit->getType() == UnitTypes::Enum::Protoss_Nexus && Broodwar->getFrameCount() < 100)
		{
			Position nexusPosition = unit->getPosition();
			furthestNexus = unit->getTilePosition();
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
	}

	// If unit not owned by player
	if (unit->getPlayer()->getID() == Broodwar->enemy()->getID())
	{
		if (unit->getType().isResourceDepot())
		{
			Broodwar << "Enemy Resource Depot Found!" << endl;
			if (enemyBasePositions.size() == 0)
			{
				enemyStartingPosition = unit->getPosition();
				enemyStartingTilePosition = TilePosition(enemyStartingPosition);
			}
			enemyBasePositions.push_back(unit->getPosition());
		}
		else if (!unit->getType().isWorker())
		{
			enemySupply = enemySupply + unit->getType().supplyRequired();
		}
		if (unit->getType().isBuilding() && enemyFound == false)
		{
			enemyStartingTilePosition = getNearestBaseLocation(unit->getPosition())->getTilePosition();
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

void CMProtoBot::drawTerrainData()
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