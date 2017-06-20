# McRave - A Broodwar Bot
## For any questions, email christianmccrave@gmail.com
## Bot started 01/03/2017, latest update 06/19/2017

A Broodwar AI Developed in C++ using Visual Studio Express 2013, BWAPI, BWTA and BWEM. This bot is focused on ending the game in the early or middle stages of the game by applying pressure through containing the opponent, using unit efficiency to take as many favorable fights and utilizing the concept of momentum to push advantages. Currently the bot can do the following:

**Information:**
- Potential fields for resources, units, structures and map based information such as mobility and ground distances.
- Optimal locations for expansions and defensive positions.

**Production:**
- Produces most units and upgrades, dependant on matchup and composition mixtures.
- Reserves resources for higher priority units, buildings, tech and upgrades.
- Expands when saturated or excessive resources.
- Adds on additional production buildings when none are idle and resources are high.

**Micromanagement:**
- Mineral locking Probes to min/max the Probe count to mining rate ratio.
- Ranged units will attempt to kite their target if the target has lower range.
- Melee units will attempt to kite their target if the unit is low health and the target is melee.
- Units with AoE attacks or abilities will target clusters.

**Strategy:**
- Uses strength based calculations dependent on unit types and momentum of dead units to make decisions.
- If attacking, chooses to explore areas where enemies recently were or to attack the closest enemy expansion.
- If retreating, it contains the enemy into their base by waiting on the perimeter or finds the safest path to a defensive position.

**Future Implementations:**
- Harassment micro
- Random race!
