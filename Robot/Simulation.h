#ifndef SIMULATION_H
#define SIMULATION_H

#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <vector>
#include "Robot.h"
#include "Map.h"
#include "FileManager.hpp"

namespace fs = std::filesystem;

class Simulation {
private:
    // Logging members
    std::stringstream logStream;
    std::streambuf* oldCoutBuffer;
    std::streambuf* oldCerrBuffer;

    Map map;
    Robot robot = Robot(0, 0, 0);

    std::vector<std::string> simulationLogs;
    void addLog(const std::string& message);
    void askToSaveLogs();

    // Check if simulation setup is valid
    bool isSimulationValid() const;

    // Simulation options
    void addRubbish(size_t tileId, unsigned int dirtiness);
    // NEW METHOD DECLARATION (already there, just confirming)
    void addSerialRubbish(unsigned int numberOfRubbishPoints);

    bool isRobotValid() const;
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

    // Private simulation methods
    bool isSimulationValid() const;
    void addRubbish(size_t tileId, unsigned int dirtiness);
    void addSerialRubbish(unsigned int numberOfRubbishPoints);
    void changeRobotsPosition(size_t newPositionId);
    void orderRobotToGoHome();
    void orderRobotToMove(size_t targetTileId);
    void orderRobotToClean(size_t tileId, unsigned int radius);
    void orderRobotToCleanEfficiently();
    void resetRobotMemory();
    void saveSimulation(fs::path filePath);
    void loadSimulation(fs::path filePath);
    void runSimulation(unsigned int steps);
    void exitSimulation();
    void printSimulation();

public:
    // Constructor
    Simulation(size_t width = 0, size_t height = 0, size_t chargerId = 0)
        : map(width, height, chargerId), robot(width, height, chargerId) {
    }

    // Main entry point
    void start(fs::path filePath = "");
    void loadFromFile(fs::path filePath);
};

#endif // SIMULATION_H