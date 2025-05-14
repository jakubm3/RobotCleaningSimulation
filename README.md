# 209.1 Robot sprzatajacy
Authours: Gabryel Jundzill, Jakub Adamczyk, Jakub Mierzejewski

Project goals:
Simulation takes path to a file as an argument which contains the plan of the room. It must be in the shape of rectangle.
Room contains floors (marked as digits), obstacles (marked as 'P') and one charger (marked as 'B'). The robot always starts from its base (charger) and moves around (in 4 directions) in order to clean trash.
Digits say how dirty is floor and how much time must be spent to clean that floor (0 means fully clean). Robot has its power level and must come back to charger to refill it.
Example of visualisation of the room (robot is marked as 'X'):
PPPPPPPPP
P0010020P
P100PP00P
P0X0PP01P
P000000BP
PPPPPPPPP

Class design:

Klasy:
Simulation:
	fields: room (collection of unique pointers to tiles), Robot robot
	methods:
		addRubbish(int howDirty=1, int id=null) - sums the howDirty value with current mess level of the tile with given id (random id if not given)
		operator<< - prints current status of map (it replaces one floor with 'X' on robot position)
		operator>> - loads new room
		findTileWithId(int id) - returns tile object with id or null
		getWidth() - returns int (width of room)
		getHeight() - returns int (height of room)
		isSimulationValid() - returns bool. Room must contain: only 1 charger; obstacle on every edge of room; every floor can be reached from base;
		runSimulation(int simulationRuns) - spawns robot in charger and calls findTarget on it. Runs until robot says it has finished. Then dependent on simulation mode new trash appears or simulation is over;

Robot:
	fields: int position (id of tile), int batteryLevel, vector<int> ignoredTiles, queue<int> path, bool finishedRun=false
	methods:
		findTarget() - returns id of target tile: Finds nearest trash; Checks if trash can be reached and if robot have enough battery to reach charger from it. 
						If yes, this trash is new target. If not, the trash is added to ignoredTiles and again checks for trash. If none found, return to base. If in base, sets finishedRun on true
		createPath(int targetId) - fills queue with IDs leading to target
		move() - moves on next point from queue; calls cleanTile(position); uses 1 energy; (if accidentally in charger recharge?)
		cleanTile(int tileId, int cleaningEfficiency=1) - calls getCleaned(cleaningEfficiency) on tile; uses 1 energy
		recharge() - refills energy; removes every Tile from ignoredTiles
		 
Tile: (asbtract)
	fields: int id (position in Simulations's collection)
	methods: 
		virtual operator<< 
		virtual isMoveValid - returns bool; true if robot can step on it

Obstacle(Tile):
	fields:
	methods:
		operator<< - outputs 'P'
		isMoveValid - returns false

Floor(Tile):
	fields: int cleanliness (value between 0-9; 0 means clean)
	methods:
		getCleaned(int efficiency=1) - reduces cleanliness
		getDirty(int howDirty=1) - increaces cleanliness
		operator<< - outputs cleanliness level
		isMoveValid - returns true

Charger(Tile):
	fields:
	methods:
		operator<< - outputs 'B'
		isMoveValid - returns true

FileManager:
	fields: simulationPath
	methods:
		operator<< streams file from simulationPath
		operator>> creates or replaces file in simulationPath


Team main responsibilities:
	- Gabryel Jundzill: merging and resolving conflicts and documentation
	- Jakub Mierzejewski: tests
	- Jakub Adamczyk: tests


TASKS:
Done:
	- Pre-documentation - Gabryel Jundzill
	- Make main file with introduction to simulation - Gabryel Jundzill
	- Create Tile template and its hierarhy (Obstacle, Floor, Charger) - Jakub Mierzejewski
	- Create Simulation class template - Jakub Adamczyk
	- Create Robot class template
	- Create file manager - Gabryel Jundzill

Current:
	- Main file has interactable interface
	- Simulation can be loaded from file
	- Validation of loaded file
	- Tiles implementation
	
Future:
	- Simulation can be runned

Milestones:
	1. Simulation can be prepared from text file
	2. Simulation can be runned and is printed on terminal
	3. Simulation have multiple run modes
