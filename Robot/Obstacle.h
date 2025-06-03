#pragma once
#include "Tile.h"

class Obstacle : public Tile {
public:
    Obstacle(size_t id = 0);

    std::unique_ptr<Tile> clone() const override;
    bool isMoveValid() const override;
};
