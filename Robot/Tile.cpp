#include "Tile.h"

Tile::Tile(size_t id) : id(id) {}

size_t Tile::getId() const noexcept {
    return id;
}

void Tile::setId(size_t newId) {
    id = newId;
}

size_t Tile::getDirection(Direction dir) const noexcept {
    return directions[dir];
}

void Tile::setDirection(Direction dir, size_t tileId) {
    directions[dir] = tileId;
}

size_t Tile::getNorth() const noexcept {
    return directions[NORTH];
}

size_t Tile::getEast() const noexcept {
    return directions[EAST];
}

size_t Tile::getSouth() const noexcept {
    return directions[SOUTH];
}

size_t Tile::getWest() const noexcept {
    return directions[WEST];
}

void Tile::setNorth(size_t tileId) {
    directions[NORTH] = tileId;
}

void Tile::setEast(size_t tileId) {
    directions[EAST] = tileId;
}

void Tile::setSouth(size_t tileId) {
    directions[SOUTH] = tileId;
}

void Tile::setWest(size_t tileId) {
    directions[WEST] = tileId;
}
