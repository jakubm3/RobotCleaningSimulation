#include "UnVisited.h"

UnVisited::UnVisited(size_t id) : Tile(id) {}

std::unique_ptr<Tile> UnVisited::clone() const {
    return std::make_unique<UnVisited>(*this);
}

bool UnVisited::isMoveValid() const {
    return false; // Robot nie może wejść na nieodwiedzony kafelek
}