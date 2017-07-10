#include "McRave.h"

void InterfaceTrackerClass::update()
{
	drawInformation();
	drawAllyInfo();
	drawEnemyInfo();
	return;
}

void InterfaceTrackerClass::performanceTest(string function)
{
	clock_t duration = clock() - globalClock;
	myTest[function] = myTest[function] * 0.99 + duration*0.01;
	if (myTest[function] > 0.0)
	{
		Broodwar->drawTextScreen(200, screenOffset, "%s", function);
		Broodwar->drawTextScreen(400, screenOffset, "%d ms", myTest[function]);
		screenOffset += 10;
	}
	return;
}

void InterfaceTrackerClass::startClock()
{
	globalClock = clock();
	return;
}

void InterfaceTrackerClass::drawInformation()
{
	int offset = 0;
	screenOffset = 0;

	// Show what buildings we want
	for (auto &b : BuildOrder().getBuildingDesired())
	{
		if (b.second > 0)
		{
			Broodwar->drawTextScreen(0, offset, "%s : %d", b.first.toString().c_str(), b.second);
			offset = offset + 10;
		}
	}

	// Display global strength calculations	
	Broodwar->drawTextScreen(500, 20, "A: %.2f    E: %.2f", Strategy().globalAlly(), Strategy().globalEnemy());

	// Display remaining minerals on each mineral patch that is near our Nexus
	for (auto &r : Resources().getMyMinerals())
	{
		Broodwar->drawTextMap(r.second.getPosition() + Position(-8, 8), "%c%d", Text::White, r.second.getRemainingResources());
	}

	// Display remaining gas on each geyser that is near our Nexus
	for (auto &r : Resources().getMyGas())
	{
		Broodwar->drawTextMap(r.second.getPosition() + Position(-8, 32), "%c%d", Text::Green, r.second.getRemainingResources());
	}
	return;
}

void InterfaceTrackerClass::drawAllyInfo()
{
	if (debugging)
	{
		for (auto &u : Units().getMyUnits())
		{
			UnitInfo unit = u.second;

			if (unit.getTargetPosition().isValid())
			{
				Broodwar->drawLineMap(unit.getTargetPosition(), unit.getPosition(), Broodwar->self()->getColor());
				Broodwar->drawBoxMap(unit.getTargetPosition() + Position(-2, -2), unit.getTargetPosition() + Position(2, 2), Broodwar->self()->getColor());
			}
			if (unit.getLocal())
			{
				Broodwar->drawTextMap(unit.getPosition() + Position(5, 0), "%c %d", unit.getPlayer()->getTextColor(), unit.getLocal());
			}
		}
	}
	return;
}

void InterfaceTrackerClass::drawEnemyInfo()
{
	if (debugging)
	{
		for (auto &u : Units().getEnUnits())
		{
			UnitInfo unit = u.second;
			if (unit.getDeadFrame() == 0)
			{
				if (unit.getStrength() > 0.0)
				{
					Broodwar->drawTextMap(unit.getPosition() + Position(5, 0), "%c %d", unit.getPlayer()->getTextColor(), unit.getStrength());
				}
			}
		}
	}
}

void InterfaceTrackerClass::sendText(string text)
{
	if (text == "/debug")
	{
		debugging = !debugging;
	}
	else
	{
		Broodwar->sendText("%s", text.c_str());
	}
	return;
}
