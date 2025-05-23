#pragma once
#include <iostream>

class Tile {
protected:
    size_t id; // pozycja w kolekcji kafelków w Room

public:
    Tile(size_t id = 0);
    virtual ~Tile() = default;

    // TODO: virtual copy constructor

    size_t getId() const noexcept;
    void setId(size_t newId);

    // Metody wirtualne, które klasy pochodne muszą zaimplementować
    virtual bool isMoveValid() const = 0;

    // Operator dla wypisywania
    /*friend std::ostream& operator<<(std::ostream& os, const Tile& tile);*/

    // Wirtualna metoda dla implementacji operatora << w klasach pochodnych
    /*virtual void print(std::ostream& os) const = 0;*/
};

// Globalna implementacja operatora <<
//std::ostream& operator<<(std::ostream& os, const Tile& tile);
