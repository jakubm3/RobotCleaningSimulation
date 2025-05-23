#pragma once
#include "Tile.h"

class Obstacle : public Tile {
public:
    Obstacle(size_t id = 0);

    // Nadpisane metody z klasy bazowej Tile
    bool isMoveValid() const override;
    /*void print(std::ostream& os) const override;*/
};
