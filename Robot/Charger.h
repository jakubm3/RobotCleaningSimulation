#pragma once
#include "Tile.h"

class Charger : public Tile {
public:
    Charger(size_t id = 0);

    std::unique_ptr<Tile> clone() const override;
    bool isMoveValid() const override;
};
