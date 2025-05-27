#include "UnVisited.h"

UnVisited::UnVisited(size_t id) : Tile(id) {}

std::unique_ptr<Tile> UnVisited::clone() const {
    return std::make_unique<UnVisited>(*this);
}

bool UnVisited::isMoveValid() const {
    return false; // Robot cannot move on unvisited tiles until they are explored
}