#include "Simulation.h"
#include <string>

bool Simulation::isSimulationValid() const {
    // TODO: Implement validation check
    return false;
}

void Simulation::addRubbish() {
    // unsigned int howDirty = 1, size_t id = static_cast<size_t>(-1)
    // TODO: Implement adding rubbish
}


void Simulation::changeRobotsPosition() {
    // TODO: set new robot position
}
void Simulation::orderRobotToGoHome() {
    // TODO: just call function XD
}
void Simulation::orderRobotToMove() {
    // TODO: order to move at
}
void Simulation::orderRobotToClean() {
    // TODO: order to clean at
}
void Simulation::resetRobotMemory() {
    // TODO: order to reset memory
}
void Simulation::saveSimulation() {
    // TODO: save to file
}
void Simulation::loadSimulation() {
    // TODO: load from file
}
void Simulation::runSimulation() {
    // TODO: run X steps, every time before step update tiles around in memory
}
void Simulation::exitSimulation() {
    // TODO: exit
}

void Simulation::printSimulation() {
    // TODO: print progress of simulation
}

void Simulation::updateRobotMemory(size_t tileId, const Tile& tileObj) {
    // TODO: update memory
}
void Simulation::cleanTile(size_t tileId, unsigned int efficiency) {
    // TODO: clean tile
}

void Simulation::start(fs::path filePath) {
    // TODO: try to load and start interface
}

//Tile* Simulation::findTileWithId(size_t id) const {
//    // TODO: Implement finding tile by ID
//    return nullptr;
//}

void Simulation::loadFromFile(fs::path filePath) {
    // TODO: load from file
}
