#include "Tile.h"

Tile::Tile(int id) : id(id) {}

int Tile::getId() const {
    return id;
}

void Tile::setId(int newId) {
    id = newId;
}

std::ostream& operator<<(std::ostream& os, const Tile& tile) {
    tile.print(os);
    return os;
}
