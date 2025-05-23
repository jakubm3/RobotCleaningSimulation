#pragma once
#include "Tile.h"

class Floor : public Tile {
private:
    unsigned int cleanliness; // Wartość między 0-9; 0 oznacza czystą podłogę

public:
    Floor(size_t id = 0, unsigned int cleanliness = 0);

    // metody dla Floor
    void getCleaned(unsigned int efficiency = 1);
    void getDirty(unsigned int howDirty = 1);
    unsigned int getCleanliness() const noexcept;
    bool isDirty() const;

    // Nadpisane metody z klasy bazowej Tile
    bool isMoveValid() const override;
    /*void print(std::ostream& os) const override;*/
};
