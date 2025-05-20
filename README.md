# 209.1 Robot sprzatajacy
Authours: Gabryel Jundzill, Jakub Adamczyk, Jakub Mierzejewski

Project goals:
Simulation loads from file map of the room and previous robot status (if no robot status, create new). Room must be in shape of rectangle.
Every floor has a cleanliness level which can be reduced by robot. Robot only sees tiles NSEW, it must first create map of the room itself.
Robot can have multiple alghorithms for moving and cleaning room.
Example of visualisation of the room (robot is marked as 'X'):
PPPPPPPPP
P0010020P
P100PP00P
P0X0PP01P
P000000BP
PPPPPPPPP

Project requirements:
1. Regular commits - done
2. Polimorfism - done (Tiles)
3. Tests - TODO
4. Collections of pointers - done (Room)
5. Printing simulation progress - TODO (Simulation)
6. Loading from file - done (Simulation)
7. Exceptions - TODO
8. unique_ptr - done (Room)
9. Template - done (FileManager)
10. Rule of five - TODO (Tiles)
11. Dynamic paths - done
12. Split responsibilities - done
13. RAII - done (FileManager)


Team main responsibilities:
	- Gabryel Jundzill: documentation, merging, project plan
	- Jakub Mierzejewski: tests
	- Jakub Adamczyk: tests
