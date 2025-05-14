#ifndef SIMULATION_H
#define SIMULATION_H

#include <memory>
#include <vector>
#include <iostream>
#include <fstream>
#include "Tile.h"
#include "Robot.h"

class Simulation {
private:
    std::vector<std::unique_ptr<Tile>> tiles;
    Robot robot;
    int width;
    int height;

public:
    // Constructor
    Simulation() : width(0), height(0) {}

    // Add rubbish to the simulation
    void addRubbish(int howDirty = 1, int id = -1);

    // Find tile with specific ID
    Tile* findTileWithId(int id);

    // Getters
    int getWidth() const { return width; }
    int getHeight() const { return height; }

    // Check if simulation setup is valid
    bool isSimulationValid() const;

    // Run the simulation
    void runSimulation(int simulationRuns);

    // Output operator
    friend std::ostream& operator<<(std::ostream& os, const Simulation& simulation);

    // Input operator
    friend std::istream& operator>>(std::istream& is, Simulation& simulation);
};

#endif // SIMULATION_H
