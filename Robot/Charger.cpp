#include "Charger.h"

Charger::Charger(size_t id) : Tile(id) {}

bool Charger::isMoveValid() const {
    return true; // Robot może wejść na ładowarkę
}

//void Charger::print(std::ostream& os) const {
//    os << 'B'; // Wypisuje 'B' jako bazę (ładowarkę)
//}
