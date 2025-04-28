#include "Floor.h"
#include <algorithm>

Floor::Floor(int id, int cleanliness) : Tile(id) {
    this->cleanliness = std::min(std::max(cleanliness, 0), 9); // poziom brudu między 0 a 9
}

void Floor::getCleaned(int efficiency) {
    cleanliness = std::max(0, cleanliness - efficiency);
}

void Floor::getDirty(int howDirty) {
    cleanliness = std::min(9, cleanliness + howDirty);
}

int Floor::getCleanliness() const {
    return cleanliness;
}

bool Floor::isDirty() const {
    return cleanliness > 0;
}

bool Floor::isMoveValid() const {
    return true; // Robot może wejść na podłogę
}

void Floor::print(std::ostream& os) const {
    os << cleanliness; // Wypisuje poziom zabrudzenia jako cyfrę
}
