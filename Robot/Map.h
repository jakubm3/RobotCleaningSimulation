#pragma once
#include <memory>
#include <vector>
#include <iostream>
#include <optional>
#include "Tile.h"
#include "Obstacle.h"
#include "Charger.h"
#include "Floor.h"
#include "UnVisited.h"

enum class Direction {
    up,
    down,
    left,
    right,
    none
};

class Map {
private:
    size_t width = 0;
    size_t height = 0;
    std::vector<std::unique_ptr<Tile>> tiles;
    size_t chargerId = 0;

public:
    // Constructors and destructor
    Map() = default;
    Map(std::istream& in);
    Map(std::istream& in, bool allowUnvisited);
    Map(size_t mapWidth, size_t mapHeight, size_t chargerTileId);

    // Rule of five
    ~Map() = default;
    Map(const Map& other);
    Map& operator=(const Map& other);
    Map(Map&& other) noexcept = default;
    Map& operator=(Map&& other) noexcept = default;

    // Getters
    size_t getWidth() const noexcept;
    size_t getHeight() const noexcept;
    size_t getChargerId() const noexcept;
    size_t getSize() const { return height * width; }
    std::optional<size_t> getIndex(size_t position, Direction direction) const;

    // Map operations
    bool isMapValid() const;
    bool isMapValid(bool allowUnvisited) const;
    bool canMoveOn(size_t tileId) const;
    void loadMap(std::istream& in);
    void loadMap(std::istream& in, bool allowUnvisited);
    void saveMap(std::ostream& os) const;
    void updateTile(size_t tileId, const Tile* tileObj);
    Tile* getTile(size_t index);
    const Tile* getTile(size_t index) const;
    Tile* getTile(size_t index, Direction direction);
    const Tile* getTile(size_t index, Direction direction) const;

    // Output operator
    friend std::ostream& operator<<(std::ostream& os, const Map& map);
};