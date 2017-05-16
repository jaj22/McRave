#pragma once
#include <BWAPI.h>
#include <BWTA.h>

using namespace BWAPI;
using namespace BWTA;
using namespace std;

class NexusInfo{
private:
	int staticDefenseCount;
	TilePosition staticPosition;
public:
	// Constructors
	NexusInfo(int, TilePosition);

	// Accessors
	int getStaticDefenseCount() const;
	TilePosition getStaticPosition() const;


	// Mutators
	void setStaticDefenseCount(int);
	void setStaticPosition(TilePosition);
};