//#include "Interface.h"
//
//void draw()
//{
//	if (masterDraw)
//	{
//		int offset = 0;
//
//		// Show what buildings we want
//		for (auto b : buildingDesired)
//		{
//			if (b.second > 0)
//			{
//				Broodwar->drawTextScreen(0, offset, "%s : %d", b.first.toString().c_str(), b.second);
//				offset = offset + 10;
//			}
//		}
//
//		// Saturated
//		Broodwar->drawTextScreen(200, 40, "%d", saturated);
//
//		// Display some information about our queued resources required for structure building			
//		Broodwar->drawTextScreen(200, 0, "Current Strategy: %s", currentStrategy.c_str());
//
//		// Display frame count
//		Broodwar->drawTextScreen(200, 10, "%d", Broodwar->getFrameCount());
//
//		// Display global strength calculations	
//		Broodwar->drawTextScreen(500, 20, "A: %.2f    E: %.2f", allyStrength, enemyStrength);
//
//		// Display remaining minerals on each mineral patch that is near our Nexus
//		for (auto r : myMinerals)
//		{
//			Broodwar->drawTextMap(r.second.getPosition() + Position(-8, 8), "%c%d", Text::White, r.second.getRemainingResources());
//		}
//
//		// Display remaining gas on each geyser that is near our Nexus
//		for (auto r : myGas)
//		{
//			Broodwar->drawTextMap(r.second.getPosition() + Position(-8, 32), "%c%d", Text::Green, r.second.getRemainingResources());
//		}
//
//		// Display static defense count
//		for (auto n : myNexus)
//		{
//			Broodwar->drawTextMap(n.first->getPosition(), "Static Defenses: %d", n.second.getStaticD());
//		}
//
//		// Display Goon/Zealot scoring
//		offset = 0;
//		for (auto t : unitScore)
//		{
//			Broodwar->drawTextScreen(500, 200 + offset, "%s : %.2f", t.first.toString().c_str(), t.second);
//			offset = offset + 10;
//		}
//
//		// Show building placements
//		for (auto b : queuedBuildings)
//		{
//			Broodwar->drawBoxMap(Position(b.second.first), Position((b.second.first.x + b.first.tileWidth()) * 32, (b.second.first.y + b.first.tileHeight()) * 32), playerColor);
//		}
//
//		// Show static defense positions
//		for (auto nexus : myNexus)
//		{
//			//Broodwar->drawCircleMap(Position(nexus.second.getStaticP()), 8, Colors::Red, true);
//		}
//
//		// Show support position
//		//Broodwar->drawCircleMap(supportPosition, 8, playerColor, true);
//
//		// Show expansions
//		if (analyzed)
//		{
//			for (int i = 0; i <= (int)activeExpansion.size() - 1; i++)
//			{
//				Broodwar->drawTextMap(48 + activeExpansion.at(i).x * 32, 104 + activeExpansion.at(i).y * 32, "%cBase %d", Broodwar->self()->getTextColor(), i, Colors::White);
//			}
//
//		}
//	}
//}