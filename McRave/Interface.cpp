#include "McRave.h"

void InterfaceTrackerClass::update()
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

	// Display frame count and APM
	//Broodwar->drawTextScreen(200, 0, "%d", Broodwar->getAPM());
	//Broodwar->drawTextScreen(200, 10, "%d", Broodwar->getFrameCount());

	// Display global strength calculations	
	Broodwar->drawTextScreen(500, 20, "A: %.2f    E: %.2f", Strategy().globalAlly(), Strategy().globalEnemy());

	// Display remaining minerals on each mineral patch that is near our Nexus
	/*for (auto &r : Resources().getMyMinerals())
	{
	Broodwar->drawTextMap(r.second.getPosition() + Position(-8, 8), "%c%d", Text::White, r.second.getRemainingResources());
	}*/

	// Display remaining gas on each geyser that is near our Nexus
	/*for (auto &r : Resources().getMyGas())
	{
	Broodwar->drawTextMap(r.second.getPosition() + Position(-8, 32), "%c%d", Text::Green, r.second.getRemainingResources());
	}	*/
	return;
}

void InterfaceTrackerClass::performanceTest(string function)
{
	double duration = (double(clock() - globalClock));
	Broodwar->drawTextScreen(200, screenOffset,"%s : %d", function, duration);
	screenOffset += 10;
	return;
}

