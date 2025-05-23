#include "Floor.h"
#include <algorithm>

Floor::Floor(size_t id, unsigned int cleanliness) : Tile(id) {
    this->cleanliness = std::min(cleanliness, 9u); // poziom brudu między 0 a 9
}

std::unique_ptr<Tile> Floor::clone() const {
    return std::make_unique<Floor>(*this);
}

void Floor::getCleaned(unsigned int efficiency) {
    if (efficiency >= cleanliness) {
        cleanliness = 0;
    }
    else {
        cleanliness -= efficiency;
    }
}

void Floor::getDirty(unsigned int howDirty) {
    cleanliness = std::min(9u, cleanliness + howDirty);
}

unsigned int Floor::getCleanliness() const noexcept {
    return cleanliness;
}

void Floor::setCleanliness(unsigned int howDirty) {
    cleanliness = std::min(howDirty, 9u); // ograniczenie do maksimum 9
}

bool Floor::isDirty() const {
    return cleanliness > 0;
}

bool Floor::isMoveValid() const {
    return true; // Robot może wejść na podłogę
}
