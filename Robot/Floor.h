#pragma once
#include "Tile.h"

class Floor : public Tile {
private:
    int cleanliness; // Wartość między 0-9; 0 oznacza czystą podłogę

public:
    Floor(int id = 0, int cleanliness = 0);

    // metody dla Floor
    void getCleaned(int efficiency = 1);
    void getDirty(int howDirty = 1);
    int getCleanliness() const;
    bool isDirty() const;

    // Nadpisane metody z klasy bazowej Tile
    bool isMoveValid() const override;
    void print(std::ostream& os) const override;
};
