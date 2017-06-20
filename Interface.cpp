#include "McRave.h"

void InterfaceTrackerClass::update()
{
	int offset = 0;

	// Show what buildings we want
	for (auto b : BuildOrder().getBuildingDesired())
	{
		if (b.second > 0)
		{
			Broodwar->drawTextScreen(0, offset, "%s : %d", b.first.toString().c_str(), b.second);
			offset = offset + 10;
		}
	}		

	// Display some information about our queued resources required for structure building			
	//Broodwar->drawTextScreen(200, 0, "Current Strategy: %s", currentStrategy.c_str());

	// Display frame count and APM
	Broodwar->drawTextScreen(200, 0, "%d", Broodwar->getAPM());
	Broodwar->drawTextScreen(200, 10, "%d", Broodwar->getFrameCount());

	// Display global strength calculations	
	Broodwar->drawTextScreen(500, 20, "A: %.2f    E: %.2f", Strategy().globalAlly(), Strategy().globalEnemy());

	// Display remaining minerals on each mineral patch that is near our Nexus
	for (auto r : Resources().getMyMinerals())
	{
		Broodwar->drawTextMap(r.second.getPosition() + Position(-8, 8), "%c%d", Text::White, r.second.getRemainingResources());
	}

	// Display remaining gas on each geyser that is near our Nexus
	for (auto r : Resources().getMyGas())
	{
		Broodwar->drawTextMap(r.second.getPosition() + Position(-8, 32), "%c%d", Text::Green, r.second.getRemainingResources());
	}	

	//// Display Goon/Zealot scoring
	//offset = 0;
	//for (auto t : unitScore)
	//{
	//	Broodwar->drawTextScreen(500, 200 + offset, "%s : %.2f", t.first.toString().c_str(), t.second);
	//	offset = offset + 10;
	//}	

	// Show expansions
	if (Terrain().getAnalyzed())
	{
		for (int i = 0; i <= (int)Terrain().getActiveExpansion().size() - 1; i++)
		{
			Broodwar->drawTextMap(48 + Terrain().getActiveExpansion().at(i).x * 32, 104 + Terrain().getActiveExpansion().at(i).y * 32, "%cBase %d", Broodwar->self()->getTextColor(), i, Colors::White);
		}

	}
	return;
}

