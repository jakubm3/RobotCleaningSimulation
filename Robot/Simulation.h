#ifndef SIMULATION_H
#define SIMULATION_H

#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream> // NEW: Needed for std::stringstream
#include "Robot.h"
#include "Map.h"
#include "FileManager.hpp"


namespace fs = std::filesystem;

class Simulation {
private:
    // --- NEW MEMBERS FOR LOGGING ---
    std::stringstream logStream;      // To capture console output
    std::streambuf* oldCoutBuffer;    // To store original cout buffer
    std::streambuf* oldCerrBuffer;    // To store original cerr buffer
    // -------------------------------

    Map map;
    Robot robot = Robot(0, 0, 0);

    std::vector<std::string> simulationLogs; // To store simple log messages
    void addLog(const std::string& message); // Helper to add messages to the log
    void askToSaveLogs();

    // Check if simulation setup is valid
    bool isSimulationValid() const;

    // Simulation options
    void addRubbish(size_t tileId, unsigned int dirtiness);
    // NEW METHOD DECLARATION (already there, just confirming)
    void addSerialRubbish(unsigned int numberOfRubbishPoints);

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
    Simulation(size_t width = 0, size_t height = 0, size_t chargerId = 0)
        : map(width, height, chargerId), robot(width, height, chargerId) {
    }

    // Opens interface
    void start(fs::path filePath = ""); // Make filePath optional for new simulations

    void loadFromFile(fs::path filePath);
};

#endif // SIMULATION_H