#include "Obstacle.h"

Obstacle::Obstacle(size_t id) : Tile(id) {}

bool Obstacle::isMoveValid() const {
    return false; // Robot nie może wejść na przeszkody
}

//void Obstacle::print(std::ostream& os) const {
//    os << 'P'; // Wypisuje 'P' jako przeszkodę
//}
