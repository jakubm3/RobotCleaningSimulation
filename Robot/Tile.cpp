#include "Tile.h"

Tile::Tile(size_t id) : id(id) {}

size_t Tile::getId() const noexcept {
    return id;
}

void Tile::setId(size_t newId) {
    id = newId;
}
