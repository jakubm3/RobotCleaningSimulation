#pragma once
#include "Tile.h"

class Obstacle : public Tile {
public:
    Obstacle(size_t id = 0);

    // Virtual copy constructor
    std::unique_ptr<Tile> clone() const override;

    // Nadpisane metody z klasy bazowej Tile
    bool isMoveValid() const override;
};
