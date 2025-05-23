#ifndef SIMULATION_H
#define SIMULATION_H

#include <iostream>
#include <fstream>
#include <filesystem>
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
    void addRubbish();
    void changeRobotsPosition();
    void orderRobotToGoHome();
    void orderRobotToMove();
    void orderRobotToClean();
    void resetRobotMemory();
    void saveSimulation();
    void loadSimulation();
    void runSimulation();
    void exitSimulation();

    void printSimulation();

    // Robots interaction
    void updateRobotMemory(size_t tileId, const Tile& tileObj);
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
