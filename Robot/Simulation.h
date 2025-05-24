#ifndef SIMULATION_H
#define SIMULATION_H

#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include "Robot.h"
#include "Map.h"
#include "FileManager.hpp"

namespace fs = std::filesystem;

class Simulation {
private:
    Map map;
    Robot robot = Robot(0, 0, 0);

    // Check if simulation setup is valid
    bool isSimulationValid() const;

    // Simulation options
    void addRubbish(size_t tileId, unsigned int dirtiness);
    void changeRobotsPosition(size_t newPositionId); // Assuming robot moves to a tile ID
    void orderRobotToGoHome(); // No parameters needed
    void orderRobotToMove(size_t targetTileId); // Robot moves to specific tile
    void orderRobotToClean(size_t tileId, unsigned int radius); // Robot cleans a specific tile with radius
    void resetRobotMemory(); // No parameters needed
    void saveSimulation(fs::path filePath); // Save to a specific file
    void loadSimulation(fs::path filePath); // Load from a specific file
    void runSimulation(unsigned int steps); // Run for N steps
    void exitSimulation(); // No parameters needed

    void printSimulation();

    // Robots interaction
    void updateRobotMemory(size_t tileId, const Tile* tileObj);
    void cleanTile(size_t tileId, unsigned int efficiency);

public:
    // Constructor
    Simulation() = default;

    // Opens interface
    void start(fs::path filePath);

    // Find tile with specific ID           chyba niepotrzebne?
    //Tile* findTileWithId(size_t id) const;

    void loadFromFile(fs::path filePath);
};

#endif // SIMULATION_H
