#pragma once
#include "Tile.h"

class Charger : public Tile {
public:
    Charger(size_t id = 0);

    // Virtual copy constructor
    std::unique_ptr<Tile> clone() const override;

    // nadpisane metody z klasy bazowej Tile
    bool isMoveValid() const override;
};
