#include "Map.h"
#include <stdexcept>
#include <string>
#include <sstream>

Map::Map(std::istream& in) {
    loadMap(in);
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

void Map::updateTile(size_t tileId, const Tile& tileObj) {
    if (tileId >= tiles.size()) {
        throw std::out_of_range("Tile ID out of range");
    }

    // If current tile is UnVisited, replace with copy
    if (dynamic_cast<const UnVisited*>(tiles[tileId].get())) {
        tiles[tileId] = tileObj.clone();
        tiles[tileId]->setId(tileId);
    }
    // If both are Floor tiles, copy cleanliness
    else if (const Floor* currentFloor = dynamic_cast<const Floor*>(tiles[tileId].get())) {
        if (const Floor* newFloor = dynamic_cast<const Floor*>(&tileObj)) {
            const_cast<Floor*>(currentFloor)->setCleanliness(newFloor->getCleanliness());
        }
    }
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

std::ostream& operator<<(std::ostream& os, const Map& map) {
    if (map.tiles.empty()) {
        os << "Map is empty.";
        return os;
    }

    for (size_t i = 0; i < map.height; ++i) {
        for (size_t j = 0; j < map.width; ++j) {
            size_t index = i * map.width + j;
            const Tile* tile = map.tiles[index].get();

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

    return os;
}