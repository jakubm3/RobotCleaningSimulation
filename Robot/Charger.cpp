#include "Charger.h"

Charger::Charger(size_t id) : Tile(id) {}

std::unique_ptr<Tile> Charger::clone() const {
    return std::make_unique<Charger>(*this);
}

bool Charger::isMoveValid() const {
    return true; // Robot może wejść na ładowarkę
}

//void Charger::print(std::ostream& os) const {
//    os << 'B'; // Wypisuje 'B' jako bazę (ładowarkę)
//}
