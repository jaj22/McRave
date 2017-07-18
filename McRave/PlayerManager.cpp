#include "McRave.h"

void PlayerTrackerClass::update()
{
	// Reset	
	eZerg = 0;
	eProtoss = 0;
	eTerran = 0;

	// Store enemy races
	for (auto &player : Broodwar->enemies())
	{
		if (player->getRace() == Races::Zerg)
		{
			eZerg++;
		}
		else if (player->getRace() == Races::Protoss)
		{
			eProtoss++;
		}
		else if (player->getRace() == Races::Terran)
		{
			eTerran++;
		}
	}
}