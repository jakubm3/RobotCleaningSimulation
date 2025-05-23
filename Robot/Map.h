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
    size_t width;
    size_t height;
    std::vector<std::unique_ptr<Tile>> tiles;
    size_t chargerId;

public:
    // Constructors and destructor
    Map() = default;
    Map(std::istream& in);
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

    // Map operations
    bool isMapValid() const;
    bool canMoveOn(size_t tileId) const;
    void loadMap(std::istream& in);
    void updateTile(size_t tileId, const Tile& tileObj);
    std::optional<size_t> getIndex(size_t position, Direction direction) const;

    // Output operator
    friend std::ostream& operator<<(std::ostream& os, const Map& map);
};