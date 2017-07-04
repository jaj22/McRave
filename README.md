# McRave - A Broodwar Bot
## For any questions, email christianmccrave@gmail.com
## Bot started 01/03/2017, latest update 07/04/2017

A Broodwar AI Developed in C++ using Visual Studio Express 2013, BWAPI and BWEM.

**Information:**
- Stores all useful unit information of enemy, neutral and ally units.
- Stores all Terrain information gathered by BWEM.
- Grids for efficient access of information such as ground distances.

**Production:**
- Follows a general build order for the start of the game
- After opening build, there are strict tech paths for each race and production buildings become independant of any build order

**Micromanagement:**
- Ranged units will kite melee units and other ranged units with lower range.
- Melee units will kite melee units if their health is low to try and stay alive.
- Transports can pickup and drop off any units to their targets position.

**Strategy:**
- Every unit has a strength metric based on health, damage and range.
- If the local strength of an area around a unit is higher, they will engage the enemy.
- If the local strength is lower, they will contain the enemy and not engage if possible.

**Future Implementations:**
- Moving fogged units instead of removing their known position.
- Harassment micro
