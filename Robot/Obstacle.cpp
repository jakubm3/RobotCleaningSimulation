#include "Obstacle.h"

Obstacle::Obstacle(size_t id) : Tile(id) {}

std::unique_ptr<Tile> Obstacle::clone() const {
    return std::make_unique<Obstacle>(*this);
}

bool Obstacle::isMoveValid() const {
    return false; // Robot nie może wejść na przeszkody
}

