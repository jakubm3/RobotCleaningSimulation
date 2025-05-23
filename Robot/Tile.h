#pragma once
#include <iostream>
#include <memory>

class Tile {
public:
    static constexpr size_t INVALID_ID = static_cast<size_t>(-1);

    enum Direction {
        NORTH = 0,
        EAST = 1,
        SOUTH = 2,
        WEST = 3
    };

protected:
    size_t id; // pozycja w kolekcji kafelków w Room
    size_t directions[4] = {INVALID_ID, INVALID_ID, INVALID_ID, INVALID_ID};

public:
    Tile(size_t id = 0);
    virtual ~Tile() = default;

    // Virtual copy constructor
    virtual std::unique_ptr<Tile> clone() const = 0;

    size_t getId() const noexcept;
    void setId(size_t newId);

    // Getters and setters for directions
    size_t getDirection(Direction dir) const noexcept;
    void setDirection(Direction dir, size_t tileId);
    size_t getNorth() const noexcept;
    size_t getEast() const noexcept;
    size_t getSouth() const noexcept;
    size_t getWest() const noexcept;
    void setNorth(size_t tileId);
    void setEast(size_t tileId);
    void setSouth(size_t tileId);
    void setWest(size_t tileId);

    // Metody wirtualne, które klasy pochodne muszą zaimplementować
    virtual bool isMoveValid() const = 0;

    // Operator dla wypisywania
    /*friend std::ostream& operator<<(std::ostream& os, const Tile& tile);*/

    // Wirtualna metoda dla implementacji operatora << w klasach pochodnych
    /*virtual void print(std::ostream& os) const = 0;*/
};

// Globalna implementacja operatora <<
//std::ostream& operator<<(std::ostream& os, const Tile& tile);
