// McRave is made by Christian McCrave
// Twitch nicknamed it McRave \o/
// For any questions, email christianmccrave@gmail.com
// Bot started 01/03/2017

#include "Header.h"
#include "McRave.h"

// --- AUTHOR NOTES ---
// Critical TODO:

// TODO in testing before CIG 2017:
// Make micro and threat grids relative to speed using 1 second intervals (if a unit moves at 8 pixels/second, they can look at 8 pixels left and right)
// Archon merging
// New distance grid
// New priority
// New strength
// New reserve path
// Delayed second scout
// Spider mine removal from expansions

// TODO:
// Move production buildings to the front of the base, tech to the back
// Dijkstras theory for distance grid
// Move stim research to strategy
// One time increases: supply, sizes
// Player class to track upgrades/race/supply/strength

// TODO to move to no latency compensation:
// Building idle status stored
// Unit idle status stored?
// Update commands to remove any latency components
void McRaveModule::onStart()
{
	Broodwar->enableFlag(Flag::UserInput);	
	Broodwar->setCommandOptimizationLevel(0);
	Broodwar->setLatCom(true);
	Broodwar->setLocalSpeed(0);
	theMap.Initialize();
	theMap.EnableAutomaticPathAnalysis();
	bool startingLocationsOK = theMap.FindBasesForStartingLocations();
	assert(startingLocationsOK);
	Terrain().onStart();
}

void McRaveModule::onEnd(bool isWinner)
{
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
	Buildings().update();
	Production().update();
	BuildOrder().update();
	Bases().update();
	Display().update();
}

void McRaveModule::onSendText(std::string text)
{
	Display().sendText(text);
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