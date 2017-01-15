// CMProtoBot is made by Christian McCrave
// For any questions, email christianmccrave@gmail.com
// Bot started 01/03/2017 - currently in Alpha

// Include 

#include "Header.h"
#include "File.h"
// Include standard libraries that are needed
#include <iostream>
#include <set>
#include <BWTA.h>
#include <vector>
#include <iterator>
#include <fstream>

// Namespaces
using namespace std;
using namespace BWAPI;
using namespace BWTA;
using namespace Filter;

bool analyzed;
bool analysis_just_finished;

TilePosition nexusTilePosition;


void CMProtoBot::onStart()
{
	// Print the map name.
	// BWAPI returns std::string when retrieving a string, don't forget to add .c_str() when printing!
	Broodwar << "The map is " << Broodwar->mapName() << "!" << endl;

	// Enable the UserInput flag, which allows us to control the bot and type messages.
	Broodwar->enableFlag(Flag::UserInput);

	// Uncomment the following line and the bot will know about everything through the fog of war (cheat).
	//Broodwar->enableFlag(Flag::CompleteMapInformation);

	// Set the command optimization level so that common commands can be grouped
	// and reduce the bot's APM (Actions Per Minute).
	Broodwar->setCommandOptimizationLevel(2);

	if (Broodwar->enemy())
	{
		Broodwar << "The matchup is " << Broodwar->self()->getRace() << " vs " << Broodwar->enemy()->getRace() << std::endl;
	}
	if (analyzed == false) {
		//Broodwar << "Analyzing map... this may take a minute" << std::endl;;
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AnalyzeThread, NULL, 0, NULL);
	}

	BWTA::readMap();
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
		playerStartingLocation = BWTA::getStartLocation(Broodwar->self());
		playerStartingPosition = playerStartingLocation->getPosition();
		playerStartingTilePosition = playerStartingLocation->getTilePosition();

		// Find all start locations
		set<BWTA::BaseLocation*> startLocations = BWTA::getStartLocations();
		for (set<BWTA::BaseLocation*>::iterator itr = startLocations.begin(); itr != startLocations.end(); itr++)
		{
			startingLocationPositions.push_back((*itr)->getPosition());
		}


		// Find base locations, find positions and tilepositions of each base location, put those in a vector for easier use
		std::set<BWTA::BaseLocation*> bases = BWTA::getBaseLocations();
		for (std::set<BWTA::BaseLocation*>::iterator itr = bases.begin(); itr != bases.end(); itr++)
		{
			// Get base positions and tile positions, store in vectors
			basePositions.push_back((*itr)->getPosition());
			baseTilePositions.push_back((*itr)->getTilePosition());
			// Get closest locations to player starting tile position
			baseDistances.push_back(BWTA::getGroundDistance(playerStartingTilePosition, (*itr)->getTilePosition()));
			baseDistancesBuffer.push_back(BWTA::getGroundDistance(playerStartingTilePosition, (*itr)->getTilePosition()));
		}

		// Find nearest bases, move to vector for easier use
		for (int i = 0; i <= (int)baseDistances.size() - 1; i++)
		{
			nearestBases.push_back(*std::min_element(baseDistancesBuffer.begin(), baseDistancesBuffer.end()));
			baseDistancesBuffer.erase(std::min_element(baseDistancesBuffer.begin(), baseDistancesBuffer.end()));


			// Reorganize base positions and tile positions into ascending order of distance
			nearestBasePositions.push_back(basePositions.at((find(baseDistances.begin(), baseDistances.end(), nearestBases.at(i)) - baseDistances.begin())));
			nearestBaseTilePositions.push_back(baseTilePositions.at((find(baseDistances.begin(), baseDistances.end(), nearestBases.at(i))) - baseDistances.begin()));
			nearestBaseTilePositionsBuffer.push_back(baseTilePositions.at((find(baseDistances.begin(), baseDistances.end(), nearestBases.at(i))) - baseDistances.begin()));
		}


		// Erase starting locations from base positions for better scouting/expanding utility
		nextExpansion.push_back(playerStartingTilePosition);
		nearestBasePositions.erase(find(nearestBasePositions.begin(), nearestBasePositions.end(), playerStartingPosition));
		nearestBaseTilePositions.erase(nearestBaseTilePositions.begin());
		startingLocationPositions.erase(find(startingLocationPositions.begin(), startingLocationPositions.end(), playerStartingPosition));

		// For each expansion, which there is base tile positions - player count
		for (int i = 0; i <= nearestBaseTilePositions.size() - 1; i++)
		{
			nearestBaseTilePositionsBuffer.erase(find(nearestBaseTilePositionsBuffer.begin(), nearestBaseTilePositionsBuffer.end(), nextExpansion.at(i)));
			//Look at remaining base locations, closest one is next expansion
			for (int j = 0; j <= nearestBaseTilePositionsBuffer.size() - 1; j++)
			{
				int rawDistance = pow(nextExpansion.at(i).x - nearestBaseTilePositionsBuffer.at(j).x, 2)
					+ pow(nextExpansion.at(i).y - nearestBaseTilePositionsBuffer.at(j).y, 2);
				expansionRawDistance.push_back(sqrt(rawDistance));
			}

			nextExpansion.push_back(nearestBaseTilePositionsBuffer.at(min_element(expansionRawDistance.begin(), expansionRawDistance.end()) - expansionRawDistance.begin()));

			// Based on the next expansion, get the distance to our starting location so we can see if any other expansions are closer
			double expectedDistance = sqrt(pow((playerStartingTilePosition.x - nextExpansion.back().x), 2) + pow((playerStartingTilePosition.y - nextExpansion.back().y), 2));

			// Measure distance to every possible expansion		
			for (int j = 0; j <= nearestBaseTilePositionsBuffer.size() - 1; j++)
			{
				int startDistance = pow(playerStartingTilePosition.x - (nearestBaseTilePositionsBuffer.at(j)).x, 2)
					+ pow(playerStartingTilePosition.y - (nearestBaseTilePositionsBuffer.at(j)).y, 2);
				expansionStartDistance.push_back(sqrt(startDistance));
			}

			// Let's check to see if there's a closer expansion than next expansion based on proximity to start location * 2
			if (2 * *min_element(expansionStartDistance.begin(), expansionStartDistance.end()) < expectedDistance)
			{
				nextExpansion.pop_back();
				nextExpansion.push_back(nearestBaseTilePositionsBuffer.at(min_element(expansionStartDistance.begin(), expansionStartDistance.end()) - expansionStartDistance.begin()));

			}
				expansionRawDistance.erase(expansionRawDistance.begin(), expansionRawDistance.end());
				expansionStartDistance.erase(expansionStartDistance.begin(), expansionStartDistance.end());
		}












		// --------------------------------------------------------------------------------------------------------------------------------------------
		// Chokepoint Locations
		// --------------------------------------------------------------------------------------------------------------------------------------------

		// Find chokepoints, move to vector for easier use 
		myChokes = BWTA::getChokepoints();
		for (std::set<BWTA::Chokepoint*>::iterator itr = myChokes.begin(); itr != myChokes.end(); itr++)
		{
			chokepointPositions.push_back((*itr)->getCenter());
			chokepointDistances.push_back(BWTA::getGroundDistance(playerStartingTilePosition, TilePosition(chokepointPositions.back())));
			chokepointDistancesBuffer1.push_back(BWTA::getGroundDistance(playerStartingTilePosition, TilePosition(chokepointPositions.back())));
		}
		// Find nearest chokepoints, move to vector for easier use
		for (int i = 0; i <= (int)chokepointDistances.size() - 1; i++)
		{
			lowestChokepointDistance.push_back(*std::min_element(chokepointDistancesBuffer1.begin(), chokepointDistancesBuffer1.end()));
			chokepointDistancesBuffer1.erase(std::min_element(chokepointDistancesBuffer1.begin(), chokepointDistancesBuffer1.end()));
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
		for (int i = 0; i <= nextExpansion.size() - 1; i++)
		{
			Broodwar->drawTextMap(nextExpansion.at(i).x * 32, nextExpansion.at(i).y * 32 + 10 * i, "Expansion %d", i, Colors::White);
		}
		Broodwar->drawTextMap(Position(((nextExpansion.at(nexusCnt).x * 32 + nextExpansion.at(nexusCnt + 1).x * 32)/2), ((nextExpansion.at(nexusCnt).y * 32 + nextExpansion.at(nexusCnt + 1).y * 32)/2)), "Holding Position", Colors::Yellow);
	}
	// --------------------------------------------------------------------------------------------------------------------------------------------
	// On-Screen Information
	// --------------------------------------------------------------------------------------------------------------------------------------------

	// Display some information about our buildings
	Broodwar->drawTextScreen(0, 0, "Building Count/Desired");
	Broodwar->drawTextScreen(0, 10, "Nexus %d  %d", nexusCnt, nexusDesired);
	Broodwar->drawTextScreen(0, 20, "Pylon %d  %d", pylonCnt, pylonDesired);
	Broodwar->drawTextScreen(0, 30, "Gate %d  %d", gateCnt, gateDesired);
	Broodwar->drawTextScreen(0, 40, "Gas %d  %d", gasCnt, gasDesired);
	Broodwar->drawTextScreen(0, 50, "Core %d  %d", coreCnt, coreDesired);
	Broodwar->drawTextScreen(0, 60, "Citadel %d  %d", citadelCnt, citadelDesired);
	Broodwar->drawTextScreen(0, 70, "Forge %d  %d", forgeCnt, forgeDesired);


	// Display some information about our queued resources required for structure building
	Broodwar->drawTextScreen(200, 0, "Queued Minerals: %d", queuedMineral);
	Broodwar->drawTextScreen(200, 10, "Queued Gas: %d", queuedGas);

	// Display some information about our units
	Broodwar->drawTextScreen(500, 20, "Unit Count");
	Broodwar->drawTextScreen(500, 30, "Probe Count: %d", probeCnt);
	Broodwar->drawTextScreen(500, 40, "Zealot Count: %d", zealotCnt);
	Broodwar->drawTextScreen(500, 50, "Dragoon Count: %d", dragoonCnt);
	Broodwar->drawTextScreen(500, 60, "Carrier Count: %d", carrierCnt);

	// Display some information for debuffing
	Broodwar->drawTextScreen(500, 200, "Mineral ID Size: %d", mineralID.size());
	Broodwar->drawTextScreen(500, 210, "Mineral Worker ID Size: %d", mineralWorkerID.size());
	Broodwar->drawTextScreen(500, 220, "Gas ID Size: %d", assimilatorID.size());
	Broodwar->drawTextScreen(500, 230, "Gas Worker ID Size: %d", gasWorkerID.size());

	// Display which worker is builder and scouter (slightly delayed)
	Broodwar->drawTextMap(builderPosition, "Builder", Colors::Yellow);
	Broodwar->drawTextMap(scouterPosition, "Scouter", Colors::Yellow);

	


	// --------------------------------------------------------------------------------------------------------------------------------------------
	// Structure Information
	// --------------------------------------------------------------------------------------------------------------------------------------------

	// Check what structures are desired based on current supplies	
	pylonDesired = (int)floor((Broodwar->self()->supplyUsed() / 14));
	gateDesired = min(3 * nexusCnt, (int)floor(Broodwar->self()->supplyUsed() / 20));
	coreDesired = min(1, (int)floor(Broodwar->self()->supplyUsed() / 36));
	citadelDesired = min(1, coreCnt);
	//stargateDesired = min(4, nexusCnt*Broodwar->self()->supplyUsed() / 130);
	//stargateDesired = min(4, (int)floor(0.00246078*exp(0.0231046*Broodwar->self()->supplyUsed())));
	//fleetBeaconDesired = min(1, stargateCnt);
	nexusDesired = min(5, (int)floor(Broodwar->self()->supplyUsed() / 120)) + 1;
	gasDesired = std::min(nexusCnt, (int)floor(Broodwar->self()->supplyUsed() / 34));

	// If a structure is required, make sure we notify production that we have an additional cost to not eat into
	queuedMineral = std::max(0, (pylonDesired - pylonCnt - pylonBuildingCnt)*(UnitTypes::Protoss_Pylon.mineralPrice()))
		+ std::max(0, (gateDesired - gateCnt - gateBuildingCnt)*(UnitTypes::Protoss_Gateway.mineralPrice()))
		+ std::max(0, (gasDesired - gasCnt - gasBuildingCnt)*(UnitTypes::Protoss_Assimilator.mineralPrice()))
		+ std::max(0, (coreDesired - coreCnt - coreBuildingCnt)*(UnitTypes::Protoss_Cybernetics_Core.mineralPrice()))
		+ std::max(0, (nexusDesired - nexusCnt - nexusBuildingCnt)*(UnitTypes::Protoss_Nexus.mineralPrice()))
		//+ std::max(0, (stargateDesired - stargateCnt - stargateBuildingCnt)*(UnitTypes::Protoss_Stargate.mineralPrice()))
		//+ std::max(0, (fleetBeaconDesired - fleetBeaconCnt - fleetBeaconBuildingCnt)*(UnitTypes::Protoss_Fleet_Beacon.mineralPrice()))
		+ std::max(0, (citadelDesired - citadelCnt - citadelBuildingCnt)*(UnitTypes::Protoss_Citadel_of_Adun.mineralPrice()))
		;

	queuedGas = //std::max(0, stargateDesired - stargateCnt - stargateBuildingCnt)*(UnitTypes::Protoss_Stargate.gasPrice())
		//+ std::max(0, (fleetBeaconDesired - fleetBeaconCnt - fleetBeaconBuildingCnt)*(UnitTypes::Protoss_Fleet_Beacon.gasPrice()))
		+std::max(0, (citadelDesired - citadelCnt - citadelBuildingCnt)*(UnitTypes::Protoss_Citadel_of_Adun.gasPrice()))
		;

	// --------------------------------------------------------------------------------------------------------------------------------------------
	// Unit Manager
	// --------------------------------------------------------------------------------------------------------------------------------------------

	// Prevent spamming by only running our onFrame once every number of latency frames.
	// Latency frames are the number of frames before commands are processed.
	if (Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0)
		return;

	// Iterate through all the units that we own
	for (auto &u : Broodwar->self()->getUnits())
	{
		// Ignore the unit if it no longer exists, is locked down, maelstrommed, stassised, loaded, not powered, stuck, not completed or is constructing something (SCV)
		if (!u->exists() || u->isLockedDown() || u->isMaelstrommed() || u->isStasised()
			|| u->isLoaded() || !u->isPowered() || u->isStuck() || !u->isCompleted() || u->isConstructing())
			continue;

		// Probe commands
		if (u->getType() == UnitTypes::Protoss_Probe)
		{
			// If no builder, this probe is now the builder
			if ((buildingWorkerID.size() < 1))
			{
				Broodwar << "Probe " << u->getID() << " is now the builder probe." << std::endl;
				buildingWorkerID.push_back(u->getID());
			}
			// Else if no scouter, this probe is now the scouter
			else if (scoutWorkerID.size() < 1)
			{
				Broodwar << "Probe " << u->getID() << " is now the scout probe." << std::endl;
				scoutWorkerID.push_back(u->getID());
			}
			// If probe ID not found
			if (std::find(probeID.begin(), probeID.end(), u->getID()) == probeID.end())
			{
				if (deadProbeID.size() > 0)
				{
					probeID.assign(find(probeID.begin(), probeID.end(), deadProbeID.back()) - probeID.begin(), u->getID());
					deadProbeID.pop_back();
				}
				// Assign as mineral worker if we need more
				else if ((int)mineralWorkerID.size() < (mineralID.size() * 2))
				{
					probeID.push_back(u->getID());
					mineralWorkerID.push_back(u->getID());
					Broodwar << "Probe " << u->getID() << " is now gathering minerals." << std::endl;
				}
				// Else assign as a gas worker if we need more
				else if ((int)gasWorkerID.size() < assimilatorID.size() * 3)
				{
					probeID.push_back(u->getID());
					Broodwar << "Probe " << u->getID() << " is now gathering gas." << std::endl;
					gasWorkerID.push_back(u->getID());
				}
				// The nexus sometimes trains an extra (if probeCnt isn't updated yet because probe is training, can be improved with probeTrainingCnt? (TEMP FIX)
				else
				{
					probeID.push_back(u->getID());
				}
			}
			// If probe ID is found, find out what it was doing and tell it to get back to work!
			else if ((std::find(probeID.begin(), probeID.end(), u->getID()) != probeID.end()) && u->isIdle())
			{
				// If probe was a mineral worker, find his mineral field pair
				if (find(mineralWorkerID.begin(), mineralWorkerID.end(), u->getID()) != mineralWorkerID.end() && mineralWorkerID.size() < mineralID.size() * 2)
				{
					// Probe split at start, otherwise pair up
					if (Broodwar->getFrameCount() < 100)
					{
						u->gather(Broodwar->getUnit(mineralID.at((find(mineralWorkerID.begin(), mineralWorkerID.end(), u->getID()) - mineralWorkerID.begin()) * 2)));
					}
					// Otherwise, assigned to a mineral patch
					u->gather(Broodwar->getUnit(mineralID.at((int)floor((find(mineralWorkerID.begin(), mineralWorkerID.end(), u->getID()) - mineralWorkerID.begin()) / 2))));
				}
				else if (find(gasWorkerID.begin(), gasWorkerID.end(), u->getID()) != gasWorkerID.end())
				{
					u->gather(Broodwar->getUnit(assimilatorID.at((int)floor((find(gasWorkerID.begin(), gasWorkerID.end(), u->getID()) - gasWorkerID.begin()) / 3))));
				}
				else
				{
					u->gather(u->getClosestUnit(IsMineralField));
				}
			}
			// Builder commands, checks what buildings are needed, if a building is needed and we have resources it goes and builds it

			if (u->getID() == buildingWorkerID.at(0))
			{
				builderPosition = u->getPosition();
				Broodwar->drawTextMap(u->getPosition(), "Builder", Colors::Yellow);
				if (pylonCnt + pylonBuildingCnt < pylonDesired && Broodwar->self()->minerals() >= UnitTypes::Protoss_Pylon.mineralPrice())
				{
					buildingManager(UnitTypes::Protoss_Pylon, u, playerStartingTilePosition);
				}
				else if (gateCnt + gateBuildingCnt < gateDesired && Broodwar->self()->minerals() >= UnitTypes::Protoss_Gateway.mineralPrice())
				{
					buildingManager(UnitTypes::Protoss_Gateway, u, playerStartingTilePosition);
				}
				else if (gasCnt + gasBuildingCnt < gasDesired && Broodwar->self()->minerals() >= UnitTypes::Protoss_Assimilator.mineralPrice())
				{
					u->build(UnitTypes::Protoss_Assimilator, gasTilePosition.back());
				}
				else if (coreCnt + coreBuildingCnt < coreDesired && Broodwar->self()->minerals() >= UnitTypes::Protoss_Cybernetics_Core.mineralPrice())
				{
					buildingManager(UnitTypes::Protoss_Cybernetics_Core, u, playerStartingTilePosition);
				}
				else if (citadelCnt + citadelBuildingCnt < citadelDesired && Broodwar->self()->minerals() >= UnitTypes::Protoss_Citadel_of_Adun.mineralPrice() && Broodwar->self()->gas() >= UnitTypes::Protoss_Citadel_of_Adun.gasPrice())
				{
					buildingManager(UnitTypes::Protoss_Citadel_of_Adun, u, playerStartingTilePosition);
				}
				else if (stargateCnt + stargateBuildingCnt < stargateDesired && Broodwar->self()->minerals() >= UnitTypes::Protoss_Stargate.mineralPrice() && Broodwar->self()->gas() >= UnitTypes::Protoss_Stargate.gasPrice())
				{
					buildingManager(UnitTypes::Protoss_Stargate, u, playerStartingTilePosition);
				}
				else if (fleetBeaconCnt + fleetBeaconBuildingCnt < fleetBeaconDesired && Broodwar->self()->minerals() >= UnitTypes::Protoss_Fleet_Beacon.mineralPrice() && Broodwar->self()->gas() >= UnitTypes::Protoss_Fleet_Beacon.gasPrice())
				{
					buildingManager(UnitTypes::Protoss_Fleet_Beacon, u, playerStartingTilePosition);
				}
				else if (nexusCnt + nexusBuildingCnt < nexusDesired && Broodwar->self()->minerals() >= UnitTypes::Protoss_Nexus.mineralPrice())
				{
					nexusManager(UnitTypes::Protoss_Nexus, u, nextExpansion.at(nexusCnt));
				}
			}
			// Scouter commands
			else if (u->getID() == scoutWorkerID.at(0) && BWTAhandling)
			{
				scouterPosition = u->getPosition();
				if (Broodwar->self()->supplyUsed() >= 18 && scouting == false)
				{
					scouting = true;
					u->move(startingLocationPositions.front());
					for (int i = 1; i <= (int)startingLocationPositions.size() - 1; i++)
					{
						u->move(startingLocationPositions.at(i), true);
					}

					/*			for (int i = 0; i <= (int)nearestBasePositions.size() - 1; i++)
								{
								u->move(nearestBasePositions.at(i), true);
								}*/
				}
			}

		} // End of worker unit commands
		// Probe production (currently builds probes if possible)

		else if (u->getType().isResourceDepot())
		{
			if (u->isIdle() && (Broodwar->self()->minerals() >= UnitTypes::Protoss_Probe.mineralPrice() + queuedMineral) && probeCnt < (int)(mineralID.size() * 2 + assimilatorID.size() * 3))
			{
				u->train(UnitTypes::Protoss_Probe);
				continue;
			}
		} // End of probe production 
		// Gateway unit production (currently builds dragoon if possible, if not, builds zealot if possible)
		else if ((u->getType() == UnitTypes::Protoss_Gateway) && u->isIdle())
		{
			if (coreCnt >= 1 && (Broodwar->self()->minerals() >= UnitTypes::Protoss_Dragoon.mineralPrice() + queuedMineral) && (Broodwar->self()->gas() >= UnitTypes::Protoss_Dragoon.gasPrice() + queuedGas) && (Broodwar->self()->supplyUsed() + UnitTypes::Protoss_Dragoon.supplyRequired() <= Broodwar->self()->supplyTotal()))
			{
				// Build order if statements in here				
				u->train(UnitTypes::Protoss_Dragoon);
				continue;
			}
			if (((Broodwar->self()->gas() < UnitTypes::Protoss_Dragoon.gasPrice() + queuedGas) && (Broodwar->self()->minerals() >= UnitTypes::Protoss_Zealot.mineralPrice() + queuedMineral) && (Broodwar->self()->supplyUsed() + UnitTypes::Protoss_Zealot.supplyRequired() <= Broodwar->self()->supplyTotal())))
			{
				// Build order if statements in here				
				u->train(UnitTypes::Protoss_Zealot);
				continue;
			}

		} // End of gateway unit production
		// Cybernetics Core research
		else if (u->getType() == UnitTypes::Protoss_Cybernetics_Core && u->isIdle())
		{
			if (dragoonCnt >= 2 && Broodwar->self()->minerals() >= UpgradeTypes::Singularity_Charge.mineralPrice() + queuedMineral && Broodwar->self()->gas() >= UpgradeTypes::Singularity_Charge.gasPrice() + queuedGas)
			{
				u->upgrade(UpgradeTypes::Singularity_Charge);
			}
		} // End of cybernetics core research

		else if (u->getType() == UnitTypes::Protoss_Fleet_Beacon && u->isIdle())
		{
			if (Broodwar->self()->minerals() >= UpgradeTypes::Carrier_Capacity.mineralPrice() + queuedMineral && Broodwar->self()->gas() >= UpgradeTypes::Carrier_Capacity.gasPrice() + queuedGas)
			{
				u->upgrade(UpgradeTypes::Carrier_Capacity);
			}
		} // End of fleet beacon research

		else if (u->getType() == UnitTypes::Protoss_Citadel_of_Adun && u->isIdle())
		{
			if (Broodwar->self()->minerals() >= UpgradeTypes::Leg_Enhancements.mineralPrice() + queuedMineral && Broodwar->self()->gas() >= UpgradeTypes::Leg_Enhancements.gasPrice() + queuedGas)
			{
				u->upgrade(UpgradeTypes::Leg_Enhancements);
			}
		} // End of fleet beacon research

		// Carrier production
		else if (u->getType() == UnitTypes::Protoss_Stargate && u->isIdle())
		{
			if (fleetBeaconCnt >= 1 && Broodwar->self()->minerals() >= UnitTypes::Protoss_Carrier.mineralPrice() + queuedMineral && Broodwar->self()->gas() >= UnitTypes::Protoss_Carrier.gasPrice() + queuedGas)
			{
				u->train(UnitTypes::Protoss_Carrier);
			}
		} // End of cybernetics core research

		// Moving army to chokepoints
		if (!u->getType().isWorker() && Broodwar->self()->supplyUsed() >= 160 && u->isIdle())
		{
			u->attack(enemyBasePositions.at(0));
		}
		// Dragoon commands
		else if (u->getType() == UnitTypes::Protoss_Dragoon && u->isIdle())
		{

				u->attack(nearestChokepointPosition.at(nexusDesired - 1));
			//	u->attack(Position(nextExpansion.at(nexusCnt).x * 32, nextExpansion.at(nexusCnt).y * 32));
			//	u->attack(Position(((nextExpansion.at(nexusCnt).x * 32 + nextExpansion.at(nexusCnt + 1).x * 32) / 2), ((nextExpansion.at(nexusCnt).y * 32 + nextExpansion.at(nexusCnt + 1).y * 32) / 2)));

		}
		// Zealot commands
		else  if (u->getType() == UnitTypes::Protoss_Zealot && u->isIdle())
		{
		
				u->attack(nearestChokepointPosition.at(nexusDesired - 1));

			//	u->attack(Position(nextExpansion.at(nexusCnt).x * 32, nextExpansion.at(nexusCnt).y * 32));
			//	u->attack(Position(((nextExpansion.at(nexusCnt).x * 32 + nextExpansion.at(nexusCnt + 1).x * 32) / 2), ((nextExpansion.at(nexusCnt).y * 32 + nextExpansion.at(nexusCnt + 1).y * 32) / 2)));

		}

		/*if (u->getType() == UnitTypes::Protoss_Carrier)
		{
		if (u->getInterceptorCount() < 8)
		{
		u->train(UnitTypes::Protoss_Interceptor);
		}
		if (carrierCnt < 4)
		{
		u->attack(nearestChokepointPosition.at(1));
		}
		if (carrierCnt >= 4 && !u->isMoving() && !u->isAttacking())
		{
		u->attack(enemyBasePositions.at(0));
		}
		}*/
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
		if (unit->getType() == UnitTypes::Protoss_Pylon)
		{
			pylonBuildingCnt++;
		}
		if (unit->getType() == UnitTypes::Protoss_Gateway)
		{
			gateBuildingCnt++;
		}
		if (unit->getType() == UnitTypes::Protoss_Cybernetics_Core)
		{
			coreBuildingCnt++;
		}
		if (unit->getType() == UnitTypes::Protoss_Nexus)
		{
			nexusBuildingCnt++;
			nexusTilePosition = unit->getTilePosition();
			nexusPosition = unit->getPosition();
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
		if (unit->getType() == UnitTypes::Protoss_Stargate)
		{
			stargateBuildingCnt++;
		}
		if (unit->getType() == UnitTypes::Protoss_Fleet_Beacon)
		{
			fleetBeaconBuildingCnt++;
		}
		if (unit->getType() == UnitTypes::Protoss_Citadel_of_Adun)
		{
			citadelBuildingCnt++;
		}

	}
}



void CMProtoBot::onUnitDestroy(BWAPI::Unit unit)
{
	if (unit->getPlayer() == Broodwar->self())
	{
		if (unit->getType() == UnitTypes::Protoss_Probe)
		{
			deadProbeID.push_back(unit->getID());
			probeCnt--;
			if (unit->getID() == scoutWorkerID.front())
			{
				scoutWorkerID.pop_back();
			}
			if (unit->getID() == buildingWorkerID.front())
			{
				buildingWorkerID.pop_back();
			}
		}
		if (unit->getType() == UnitTypes::Protoss_Zealot)
		{
			zealotCnt--;
		}
		if (unit->getType() == UnitTypes::Protoss_Dragoon)
		{
			dragoonCnt--;
		}
	}
}

void CMProtoBot::onUnitMorph(BWAPI::Unit unit)
{
	if (unit->getPlayer() == Broodwar->self())
	{
		if (unit->getType() == UnitTypes::Protoss_Assimilator)
		{
			gasBuildingCnt++;
		}
	}
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
	// If player owned unit
	if (unit->getPlayer() == Broodwar->self())
	{
		if (unit->getType() == UnitTypes::Protoss_Probe)
		{
			probeCnt++;
		}
		if (unit->getType() == UnitTypes::Protoss_Zealot)
		{
			zealotCnt++;
		}
		if (unit->getType() == UnitTypes::Protoss_Dragoon)
		{
			dragoonCnt++;
		}
		if (unit->getType() == UnitTypes::Protoss_Carrier)
		{
			carrierCnt++;
		}
		if (unit->getType() == UnitTypes::Protoss_Nexus)
		{
			nexusCnt++;
			nexusTilePosition = unit->getTilePosition();
			nexusPosition = unit->getPosition();
			if (Broodwar->getFrameCount() < 100)
			{
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
			else
			{
				nexusBuildingCnt--;
			}
		}
		if (unit->getType() == UnitTypes::Protoss_Pylon)
		{
			pylonCnt++;
			pylonBuildingCnt--;
		}
		if (unit->getType() == UnitTypes::Protoss_Gateway)
		{
			gateCnt++;
			gateBuildingCnt--;
		}
		if (unit->getType() == UnitTypes::Protoss_Assimilator)
		{
			gasCnt++;
			gasBuildingCnt--;
			assimilatorID.push_back(unit->getID());
		}
		if (unit->getType() == UnitTypes::Protoss_Cybernetics_Core)
		{
			coreCnt++;
			coreBuildingCnt--;
		}
		if (unit->getType() == UnitTypes::Protoss_Stargate)
		{
			stargateCnt++;
			stargateBuildingCnt--;
		}
		if (unit->getType() == UnitTypes::Protoss_Fleet_Beacon)
		{
			fleetBeaconBuildingCnt--;
			fleetBeaconCnt++;
		}
		if (unit->getType() == UnitTypes::Protoss_Citadel_of_Adun)
		{
			citadelBuildingCnt--;
			citadelCnt++;
		}
	}
	// If unit not owned by player
	if (unit->getPlayer()->getID() == Broodwar->enemy()->getID())
	{
		if (unit->getType().isResourceDepot())
		{
			Broodwar << "Enemy Resource Depot Found!" << endl;
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