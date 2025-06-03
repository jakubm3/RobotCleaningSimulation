#include "Map.h"
#include <stdexcept>
#include <string>
#include <sstream>

Map::Map(std::istream& in) {
    loadMap(in, false);
}

Map::Map(std::istream& in, bool allowUnvisited) {
    loadMap(in, allowUnvisited);
}

Map::Map(size_t mapWidth, size_t mapHeight, size_t chargerTileId)
    : width(mapWidth), height(mapHeight), chargerId(chargerTileId) {
    tiles.reserve(width * height);
    for (size_t i = 0; i < width * height; ++i) {
        if (i == chargerTileId) {
            tiles.push_back(std::make_unique<Charger>(i));
        } else {
            tiles.push_back(std::make_unique<UnVisited>(i));
        }
    }
}

// Copy constructor
Map::Map(const Map& other)
    : width(other.width), height(other.height), chargerId(other.chargerId) {
    tiles.reserve(other.tiles.size());
    for (const auto& tile : other.tiles) {
        tiles.push_back(tile->clone());
    }
}

// Copy assignment operator
Map& Map::operator=(const Map& other) {
    if (this != &other) {
        width = other.width;
        height = other.height;
        chargerId = other.chargerId;
        tiles.clear();
        tiles.reserve(other.tiles.size());
        for (const auto& tile : other.tiles) {
            tiles.push_back(tile->clone());
        }
    }
    return *this;
}

size_t Map::getWidth() const noexcept {
    return width;
}

size_t Map::getHeight() const noexcept {
    return height;
}

size_t Map::getChargerId() const noexcept {
    return chargerId;
}

bool Map::isMapValid() const {
    if (tiles.size() != width * height) {
        return false;
    }

    size_t chargerCount = 0;
    for (const auto& tile : tiles) {
        if (dynamic_cast<const Charger*>(tile.get())) {
            chargerCount++;
        }
    }

    return chargerCount == 1;
}

bool Map::canMoveOn(size_t tileId) const {
    if (tileId >= tiles.size()) {
        return false;
    }
    return tiles[tileId]->isMoveValid();
}

void Map::loadMap(std::istream& in) {
    loadMap(in, false);
}

void Map::loadMap(std::istream& in, bool allowUnvisited) {
    tiles.clear();
    width = 0;
    height = 0;
    chargerId = Tile::INVALID_ID;

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(in, line)) {
        if (!line.empty()) {
            lines.push_back(line);
        }
    }

    if (lines.empty()) {
        throw std::runtime_error("Map file is empty.");
    }

    height = lines.size();
    width = lines[0].length();

    size_t idCounter = 0;
    for (size_t i = 0; i < height; ++i) {
        if (lines[i].length() != width) {
            throw std::runtime_error("Map is not rectangular - row " + std::to_string(i) + " has different length.");
        }
        for (size_t j = 0; j < width; ++j) {
            char tileChar = lines[i][j];
            if (isdigit(tileChar)) {
                tiles.push_back(std::make_unique<Floor>(idCounter, tileChar - '0'));
            }
            else if (tileChar == 'P') {
                tiles.push_back(std::make_unique<Obstacle>(idCounter));
            }
            else if (tileChar == 'B') {
                tiles.push_back(std::make_unique<Charger>(idCounter));
                if (chargerId != Tile::INVALID_ID) {
                    throw std::runtime_error("Map contains more than one charger.");
                }
                chargerId = idCounter;
            }
            else if (tileChar == '?' && allowUnvisited) {
                tiles.push_back(std::make_unique<UnVisited>(idCounter));
            }
            else {
                throw std::runtime_error("Invalid character in map file: " + std::string(1, tileChar));
            }
            idCounter++;
        }
    }

    if (chargerId == Tile::INVALID_ID) {
        throw std::runtime_error("Map must contain exactly one charger.");
    }
}

void Map::updateTile(size_t tileId, const Tile* tileObj) {
    if (tileId >= tiles.size()) {
        throw std::out_of_range("Tile ID out of range");
    }

    // Always replace the tile with a copy of the given tile
    tiles[tileId] = tileObj->clone();
    tiles[tileId]->setId(tileId);
}

std::optional<size_t> Map::getIndex(size_t position, Direction direction) const {
    if (position >= tiles.size()) {
        return std::nullopt;
    }

    size_t row = position / width;
    size_t col = position % width;

    switch (direction) {
        case Direction::up:
            if (row == 0) return std::nullopt;
            return (row - 1) * width + col;

        case Direction::down:
            if (row >= height - 1) return std::nullopt;
            return (row + 1) * width + col;

        case Direction::left:
            if (col == 0) return std::nullopt;
            return row * width + (col - 1);

        case Direction::right:
            if (col >= width - 1) return std::nullopt;
            return row * width + (col + 1);

        case Direction::none:
        default:
            return position;
    }
}

Tile* Map::getTile(size_t index) {
    if (index < tiles.size()) {
        return tiles[index].get();
    }
    return nullptr;
}


const Tile* Map::getTile(size_t index) const {
    if (index < tiles.size()) {
        return tiles[index].get();
    }
    return nullptr;
}

Tile* Map::getTile(size_t index, Direction direction) {
    std::optional<size_t> tileIndex = getIndex(index, direction);
    if (tileIndex) {
        return getTile(*tileIndex);
    }
    return nullptr;
}

const Tile* Map::getTile(size_t index, Direction direction) const {
    std::optional<size_t> tileIndex = getIndex(index, direction);
    if (tileIndex) {
        return getTile(*tileIndex);
    }
    return nullptr;
}

void Map::saveMap(std::ostream& os) const {
    if (tiles.empty()) {
        os << "Map is empty.";
        return;
    }

    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j) {
            size_t index = i * width + j;
            const Tile* tile = tiles[index].get();

            if (const Floor* floor = dynamic_cast<const Floor*>(tile)) {
                os << static_cast<char>('0' + floor->getCleanliness());
            }
            else if (dynamic_cast<const Obstacle*>(tile)) {
                os << 'P';
            }
            else if (dynamic_cast<const Charger*>(tile)) {
                os << 'B';
            }
            else if (dynamic_cast<const UnVisited*>(tile)) {
                os << '?';
            }
            else {
                os << '?';
            }
        }
        os << '\n';
    }
}

std::ostream& operator<<(std::ostream& os, const Map& map) {
    map.saveMap(os);
    return os;
}