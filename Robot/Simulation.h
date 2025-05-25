#ifndef SIMULATION_H
#define SIMULATION_H

#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include "Robot.h"
#include "Map.h"
#include "FileManager.hpp"

// Don't forget to include these for random number generation in the .cpp,
// and potentially for std::optional and std::tuple if your Robot.h or
// other parts of your code use them directly.
// #include <random>
// #include <chrono>
// #include <optional>
// #include <tuple>


namespace fs = std::filesystem;

class Simulation {
private:
    Map map;
    Robot robot = Robot(0, 0, 0);

    // Check if simulation setup is valid
    bool isSimulationValid() const;

    // Simulation options
    void addRubbish(size_t tileId, unsigned int dirtiness);
    // NEW METHOD DECLARATION
    void addSerialRubbish(unsigned int numberOfRubbishPoints, unsigned int maxDirtiness);

    void changeRobotsPosition(size_t newPositionId); // Assuming robot moves to a tile ID
    void orderRobotToGoHome(); // No parameters needed
    void orderRobotToMove(size_t targetTileId); // Robot moves to specific tile
    void orderRobotToClean(size_t tileId, unsigned int radius); // Robot cleans a specific tile with radius
    void orderRobotToCleanEfficiently(); // NEW: Declare this function here!
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
    // It's a good idea to have a constructor that can initialize map and robot
    // with meaningful defaults or based on input, especially if you plan to
    // create a new simulation without loading from a file.
    Simulation(size_t width = 0, size_t height = 0, size_t chargerId = 0)
        : map(width, height, chargerId), robot(width, height, chargerId) {
    }

    // Original default constructor, now the one above handles both cases
    // Simulation() = default;

    // Opens interface
    void start(fs::path filePath = ""); // Make filePath optional for new simulations

    void loadFromFile(fs::path filePath);
};

#endif // SIMULATION_H