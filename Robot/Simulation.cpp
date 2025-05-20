#include "Simulation.h"
#include "Obstacle.h"
#include "Floor.h"
#include "Charger.h"
#include <string>

void Simulation::addRubbish(int howDirty, int id) {
    // TODO: Implement adding rubbish
}

Tile* Simulation::findTileWithId(int id) {
    // TODO: Implement finding tile by ID
    return nullptr;
}

bool Simulation::isSimulationValid() const {
    // TODO: Implement validation check
    return false;
}

void Simulation::runSimulation(int simulationRuns) {
    // TODO: Implement simulation runs
}

std::ostream& operator<<(std::ostream& os, const Simulation& simulation) {
    if (simulation.tiles.empty()) {
        os << "Simulation is empty.";
        return os;
    }

    for (int i = 0; i < simulation.height; ++i) {
        for (int j = 0; j < simulation.width; ++j) {
            int index = i * simulation.width + j;
            const Tile* tile = simulation.tiles[index].get();

            if (const Floor* floor = dynamic_cast<const Floor*>(tile)) {
                os << static_cast<char>('0' + floor->getCleanliness());
            }
            else if (dynamic_cast<const Obstacle*>(tile)) {
                os << 'P';
            }
            else if (dynamic_cast<const Charger*>(tile)) {
                os << 'B';
            }
            else {
                os << '?';
            }
        }
        os << '\n';
    }

    return os;
}


std::istream& operator>>(std::istream& is, Simulation& simulation) {
    simulation.tiles.clear();
    simulation.width = 0;
    simulation.height = 0;
    simulation.chargerId = -1;

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(is, line)) {
        if (!line.empty()) {
            lines.push_back(line);
        }
    }

    if (lines.empty()) {
        throw std::runtime_error("Map file is empty.");
    }

    simulation.height = lines.size();
    simulation.width = lines[0].length();

    int idCounter = 0;
    for (size_t i = 0; i < simulation.height; ++i) {
        if (lines[i].length() != simulation.width) {
            throw std::runtime_error("Map is not rectangular.");
        }
        for (size_t j = 0; j < simulation.width; ++j) {
            char tileChar = lines[i][j];
            if (isdigit(tileChar)) {
                simulation.tiles.push_back(std::make_unique<Floor>(idCounter, tileChar - '0'));
            }
            else if (tileChar == 'P') {
                simulation.tiles.push_back(std::make_unique<Obstacle>(idCounter));
            }
            else if (tileChar == 'B') {
                simulation.tiles.push_back(std::make_unique<Charger>(idCounter));
                simulation.chargerId = idCounter;
            }
            else {
                throw std::runtime_error("Invalid character in map file: " + std::string(1, tileChar));
            }
            idCounter++;
        }
    }

    if (simulation.chargerId == -1) {
        throw std::runtime_error("Map must contain exactly one charger.");
    }

    return is;
}
