# 209.1 Robot sprzatajacy
Authours: Gabryel Jundzill, Jakub Adamczyk, Jakub Mierzejewski

1. About project:
This project contains robot simulation that cleans house. To run simulation you need to launch in command line ".\Robot.exe" if
you are in folder with program. You can parse with it path to save file you want to use. Otherwise you will be asked to give file path
after launch. To create valid map file you must create a room in txt file in shape of rectangle. Below is an example:
P - obstacles
0-9 digits - floors; the higher digit, the more dirty is tile
B - charger; can be only one per map
PPPPPPPPP
P0010020P
P100PP000
P060PP010
PPP0000B1
PPPPPPPPP

After succesfully loaded map, you will see the interface of simulation. You have there multiple options. When selected option
'run simulation' the robot will start its work for X steps. Robots default objectives are in order: explore the house, 
clean the house, return to charger. This behaviour is overrided if given some orders. Robot might stop working if: placed in
wrong place; can't reach charger; order to move to invalid position was given. Robot has its own memory of map and even if he doesn't
know the path to destination, it will search for it.

2. Used elements of STL library:
filesystem - used for paths and validation
iostream, sstream, fstream, string - used for processing inputs and outputs
vector - used to store collections of objects
queue - used to store path of robot
stack - used in bfs alghorithms
tuple - used to return type of action and its direction
utility - to create tuples 
optional - handling potentially missing values

3. Use of exceptions:
Robot:
- During loading invalid robot: handled in Simulation (created default robot)
- Being on invalid tile: handled in Simulation (stopped simulation)
- Ordered to move on tile out of robot's range: handled in Simulation (stopped simulation)

Simulation:
- Invalid robot during loading: handled by creating default robot
- Robot on invalid tile: stops simulation
- Robot ordered to move out of range: stops simulation
- File operations: handled with error messages
- Invalid user input: prompts for retry

4. Tests:








Project requirements:
1. Regular commits - done
2. Polimorfism - done (Tiles)
3. Tests - TODO
4. Collections of pointers - done (Room)
5. Printing simulation progress - done (Simulation)
6. Loading from file - done (Simulation)
7. Exceptions - done
8. unique_ptr - done (Room)
9. Template - done (FileManager)
10. Rule of five - done (Tiles)
11. Dynamic paths - done
12. Split responsibilities - done
13. RAII - done (FileManager)


Team main responsibilities:
	- Gabryel Jundzill: documentation, merging, project plan
	- Jakub Mierzejewski: tests
	- Jakub Adamczyk: tests
