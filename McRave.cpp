// McRave is made by Christian McCrave
// Twitch nicknamed it McRave \o/
// For any questions, email christianmccrave@gmail.com
// Bot started 01/03/2017

#include "Header.h"
#include "McRave.h"

// --- AUTHOR NOTES ---
// DISABLED CURRENTLY: Cannons
// Critical TODOS:
// Re-check all grids!!! - Specifically the center of t/w/positions, is it the center or top left corner?
// Add getCenter() to UnitInfo?
// Store neutral untis and buildings

// Other TODOS:
// Move to BWEM and ditch BWTA, too slow
// Move production buildings to the front of the base, tech to the back
// Dijkstras theory for distance grid
// Move stim research to strategy
// If cannon was built in region, add to that Nexus
// Limit building number by region? Make pylons next to expansions for cannons/gateways
// Store the closest worker when ready to move to build position
// Worker manager can have a function to check if worker has a building assigned to it and move to it/build it

// Testing:
// Spider mine removal from expansions - Testing 2.0
// Reavers flee into shuttles

// Possibility:
// 3 base carrier against iron?
// Take angles into account for micro?
// Player class to track upgrades/race/supply/strength?
// One time supply increase instead of resetting?

void McRaveModule::onStart()
{
	// Enable the UserInput flag, which allows us to control the bot and type messages.
	Broodwar->enableFlag(Flag::UserInput);

	// Set the command optimization level so that common commands can be grouped and reduce the bot's APM (Actions Per Minute).
	Broodwar->setCommandOptimizationLevel(0);

	Broodwar->setLatCom(true);
	Broodwar->setLocalSpeed(0);

	theMap.Initialize();
	theMap.EnableAutomaticPathAnalysis();
	bool startingLocationsOK = theMap.FindBasesForStartingLocations();
	assert(startingLocationsOK);

	BWEM::utils::MapPrinter::Initialize(&theMap);
	BWEM::utils::printMap(theMap);      // will print the map into the file <StarCraftFolder>bwapi-data/map.bmp
	BWEM::utils::pathExample(theMap);   // add to the printed map a path between two starting locations

	if (Terrain().isAnalyzed() == false) {
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AnalyzeThread, NULL, 0, NULL);
	}
	readMap();
}

void McRaveModule::onEnd(bool isWinner)
{
	// Called when the game ends
	if (isWinner)
	{
		// Log your win here!
	}
}

void McRaveModule::onFrame()
{
	Terrain().update();
	Grids().update();
	Resources().update();
	Strategy().update();
	Workers().update();
	Units().update();
	SpecialUnits().update();
	Transport().update();
	Commands().update();
	BuildOrder().update();
	Buildings().update();
	Production().update();
	Bases().update();
	Display().update();
}

void McRaveModule::onSendText(std::string text)
{
	// Else send the text to the game if it is not being processed
	Broodwar->sendText("%s", text.c_str());
}

void McRaveModule::onReceiveText(BWAPI::Player player, std::string text)
{
}

void McRaveModule::onPlayerLeft(BWAPI::Player player)
{
	Broodwar->sendText("GG %s!", player->getName().c_str());
}

void McRaveModule::onNukeDetect(BWAPI::Position target)
{
}

void McRaveModule::onUnitDiscover(BWAPI::Unit unit)
{
}

void McRaveModule::onUnitEvade(BWAPI::Unit unit)
{
}

void McRaveModule::onUnitShow(BWAPI::Unit unit)
{
}

void McRaveModule::onUnitHide(BWAPI::Unit unit)
{
}

void McRaveModule::onUnitCreate(BWAPI::Unit unit)
{
	Buildings().updateQueue(unit);
}

void McRaveModule::onUnitDestroy(BWAPI::Unit unit)
{
	Units().decayUnit(unit);
	Buildings().removeBuilding(unit);
	SpecialUnits().removeUnit(unit);
	Workers().removeWorker(unit);
	Resources().removeResource(unit);
	Terrain().removeTerritory(unit);
}

void McRaveModule::onUnitMorph(BWAPI::Unit unit)
{
	Buildings().updateQueue(unit);
}

void McRaveModule::onUnitRenegade(BWAPI::Unit unit)
{
}

void McRaveModule::onSaveGame(std::string gameName)
{
}

void McRaveModule::onUnitComplete(BWAPI::Unit unit)
{
}

DWORD WINAPI AnalyzeThread()
{
	BWTA::analyze();
	Terrain().setAnalyzed();
	return 0;
}

void McRaveModule::drawTerrainData()
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